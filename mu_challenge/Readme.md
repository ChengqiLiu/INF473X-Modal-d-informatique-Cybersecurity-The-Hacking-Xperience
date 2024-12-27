In the codes, there is a "buffer overflow" problem. 
The "greeting_text[128]" can only contain 128 characters, which is not enough. If the user input is large enough, it will overwrite 
the subsequent address space whitch contains command[]. Then, this part of input will be executed in "system(command)",
which means that the user can execute almost any code with root privileges on VM. The length of "Hello, dear " is 12. So, first type 
128-12=116 characters (repeated '0'), then type any commands which need to be executed.

I used the input:
00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000for i in $(find / -name index.html);do echo '<p>Chengqi Liu</p>' >>  $i;done

"for i in $(find / -name index.html)" will find all the files named "index.html" (usually the file name of the home page), and 
"do echo '<p>Chengqi Liu</p>' >>  $i" will insert my name at the end of the file.

(In fact, the page file is in the folder "/var/www/html/", so it can be done directly by inputing:
00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000echo '<p>Chengqi Liu</p>' >> /var/www/html/index.html
This will make the attack more precise.)

The file "reverse_terminal.c" simulates the behavior of a terminal on the server, allowing the attacker to open a terminal with root 
privileges directly on the server to operate it. It can make it easier for the attacker to quickly and conveniently launch the attacks.