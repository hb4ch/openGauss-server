#!/usr/bin/env perl
use strict;
use warnings;
use File::Basename;

my $DIR = shift || 'src';
my @files;
if (-d $DIR) {
    @files = `find $DIR -name '*.cpp' -o -name '*.h' -o -name '*.c'`;
    chomp @files;
} else { @files = ($DIR); }
print "Processing " . scalar(@files) . " files...\n";
my $total_modified = 0;

my %skip = map { $_ => 1 } (
    'securec.h', 'securec_check.h', 'securectype.h', 'elog.h', 'gs_malloc.h',
);
sub is_skip_file {
    my ($f) = @_; my $bn = basename($f);
    return 1 if $skip{$bn};
    return 1 if $f =~ m{/gtm/utils/elog\.h$};
    return 1 if $f =~ m{/workload/ctxctl\.h$};
    return 0;
}

my $A = qr/(?:[^(),]+|\([^()]*+\))+/;  # non-comma-non-paren chars or balanced parens

foreach my $file (@files) {
    next if is_skip_file($file);
    open(my $fh, '<', $file) or next;
    my $content = do { local $/; <$fh> };
    close($fh);
    my $orig = $content;

    # Each regex: OLD_NAME(a1, a2, ..., aN)(trailing) -> NEW_NAME(...args...)(trailing)
    # Capture groups: $1..$N = args, $(N+1) = trailing

    # snprintf_s(d, dm, c, fmt, ...rest...)trail
    #   $1=d $2=dm $3=c(DROP) $4=fmt $5=rest $6=trail
    $content =~ s/snprintf_s\(($A),\s*($A),\s*($A),\s*($A)((?:\s*,\s*$A)*)\)(.*)$/snprintf($1, $2, $4$5)$6/gm;

    # vsnprintf_s(d, dm, c, fmt, va)trail -> $6=trail
    $content =~ s/vsnprintf_s\(($A),\s*($A),\s*($A),\s*($A),\s*($A)\)(.*)$/vsnprintf($1, $2, $4, $5)$6/gm;

    # sprintf_s(d, dm, fmt, ...rest...)trail -> $5=trail
    $content =~ s/sprintf_s\(($A),\s*($A),\s*($A)((?:\s*,\s*$A)*)\)(.*)$/snprintf($1, $2, $3$4)$5/gm;

    # vsprintf_s(d, dm, fmt, va)trail -> $5=trail
    $content =~ s/vsprintf_s\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/vsnprintf($1, $2, $3, $4)$5/gm;

    # snprintf_truncated_s(d, dm, fmt, ...rest...)trail -> $5=trail
    $content =~ s/snprintf_truncated_s\(($A),\s*($A),\s*($A)((?:\s*,\s*$A)*)\)(.*)$/snprintf($1, $2, $3$4)$5/gm;
    $content =~ s/vsnprintf_truncated_s\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/vsnprintf($1, $2, $3, $4)$5/gm;

    # memcpy_s(d, dm, s, c)trail -> $5=trail
    $content =~ s/memcpy_s\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/memcpy($1, $3, $4)$5/gm;
    $content =~ s/memcpy_sp\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/memcpy($1, $3, $4)$5/gm;
    for my $fn (qw(memcpy_sOptAsm memcpy_sOptTc)) {
        $content =~ s/$fn\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/memcpy($1, $3, $4)$5/gm;
    }

    # memmove_s(d, dm, s, c)trail -> $5=trail
    $content =~ s/memmove_s\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/memmove($1, $3, $4)$5/gm;
    $content =~ s/wmemmove_s\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/wmemmove($1, $3, $4)$5/gm;

    # memset_s(d, dm, ch, n)trail -> $5=trail
    for my $fn (qw(memset_s memset_sp memset_sOptAsm memset_sOptTc)) {
        $content =~ s/$fn\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/memset($1, $3, $4)$5/gm;
    }

    # strcpy_s(d, dm, s)trail -> $4=trail
    $content =~ s/strcpy_s\(($A),\s*($A),\s*($A)\)(.*)$/snprintf($1, $2, "%s", $3)$4/gm;
    # strncpy_s(d, dm, s, c)trail -> $5=trail
    $content =~ s/strncpy_s\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/snprintf($1, $2, "%s", $3)$5/gm;
    # strcpy_sp(d, dm, s)trail -> $4=trail
    $content =~ s/strcpy_sp\(($A),\s*($A),\s*($A)\)(.*)$/snprintf($1, $2, "%s", $3)$4/gm;
    # strncpy_sp(d, dm, s, c)trail -> $5=trail
    $content =~ s/strncpy_sp\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/snprintf($1, $2, "%s", $3)$5/gm;

    # strcat_s(d, dm, s)trail -> $4=trail
    $content =~ s/strcat_s\(($A),\s*($A),\s*($A)\)(.*)$/strncat($1, $3, $2 - 1)$4/gm;
    # strncat_s(d, dm, s, c)trail -> $5=trail
    $content =~ s/strncat_s\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/strncat($1, $3, $4)$5/gm;

    # scanf family
    $content =~ s/\bscanf_s\(/scanf(/g;
    $content =~ s/\bsscanf_s\(/sscanf(/g;
    $content =~ s/\bfscanf_s\(/fscanf(/g;
    $content =~ s/\bvscanf_s\(/vscanf(/g;
    $content =~ s/\bvsscanf_s\(/vsscanf(/g;
    $content =~ s/\bvfscanf_s\(/vfscanf(/g;

    # strtok_s -> strtok_r
    $content =~ s/\bstrtok_s\(/strtok_r(/g;

    # gets_s(d, m)trail -> $3=trail
    $content =~ s/gets_s\(($A),\s*($A)\)(.*)$/fgets($1, $2, stdin)$3/gm;

    # wmemcpy_s(d, dm, s, c)trail -> $5=trail
    $content =~ s/wmemcpy_s\(($A),\s*($A),\s*($A),\s*($A)\)(.*)$/wmemcpy($1, $3, $4)$5/gm;

    # Remove securec_check lines entirely (not comment, since they may have \ continuations)
    my $CHK = qr{
        securec_check(?:_c|_ss_c?)? |
        securec_check_for_sscanf_s |
        freeSecurityFuncSpace(?:_c)? |
        securec_check_intval_core |
        check_strncpy_s
    }x;
    $content =~ s/^[ \t]*$CHK\(((?:[^()]++|\([^()]*+\))*)\)[ \t]*;?[ \t]*(\\[ \t]*)?\n//gm;
    # Handle: ;\s*securec_check(...); (same line)
    $content =~ s/;\s*($CHK)\(((?:[^()]++|\([^()]*+\))*)\)\s*;/;/g;

    if ($content ne $orig) {
        open(my $out, '>', $file) or warn "Cannot write $file: $!\n" and next;
        print $out $content;
        close($out);
        print "  $file\n";
        $total_modified++;
    }
}
print "\nDone. $total_modified files modified.\n";
