#!/usr/bin/perl
# programme   : includeTaggedFile.pl
# author      : Florian Muecke, 04.12.2007
# description : The script opens the file passed as first parameter and 
#               searches for a line containing $tag. Then it imports the file
#               specified between the two $tag fields instead of that line.
#               This procedure done is recursively!
#               The output is written to stdout!
#
#               Replaces PlaceMainHeaderWithNavbar.sh shell script.
#
# example     : INCLUDE_doc/menubar.txt_INCLUDE

$tag = "INCLUDE";

$content = &getIncludedContent(@ARGV[0]);

sub getIncludedContent 
{
    local $content;
    local @raw;
    # open file that is passed as parameter
    # print usage and exit if too few parameter
    open( FILE, $_[0] ) or die "Can't find file $FILE: $!\n";
    @raw = <FILE>;
    close( FILE );

    foreach $line (@raw)
    {
        $tmp = substr( $line, 0, length($tag) );
        if ( $tmp eq $tag )
        {
            $inputfile = substr( $line, length($tag) + 1, length($line) - 2*length($tag) - 3 );
            $content .= &getIncludedContent($inputfile);
            $content .=  "\n";
        }
        else 
        {
            $content .= $line;
        }
    }
    $content;
}

# write to stdout
print $content;

# if you want the text inserted in the file uncomment this
#open(FILE, ">@ARGV[0]") or die "Can't open file $FILE: $!\n";
#print FILE $content;
#close(FILE);

