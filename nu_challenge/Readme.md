The idea of ​​the attack is to use buffer overflow to overwrite the return address in the stack in the function parse(). 
The following machine code can make the program execute "/bin/sh"[1][2]. Therefore, we only need to write it in buf, and then overwrite the return address in the stack with the address of buf to run the shell. The rest of the steps are the same as mu_challenge (input the command "echo '<p>Chengqi Liu</p>' >> /var/www/html/index.html").
[1] https://www.hackingarticles.in/64-bit-linux-assembly-and-shellcoding/
[2] https://gist.github.com/grittyninja/64cf24c311acf7cd1ec652ebefec59ed
The attacked server is running on a 64-bit lubuntu system, so we should use "\x31\xc0\x48\xbb\xd1\x9d\x96\x91\xd0\x8c\x97\xff\x48\xf7\xdb\x53\x54\x5f\x99\x52\x57\x54\x5e\xb0\x3b\x0f\x05".

shell.c is to run the shell code to see whether it works.
test1.c is to test a simplified version of the source code of mu_challenge.
test2.c is to test a simplified version of the source code of nu_challenge. (I used the gdb commands "p &greeting_text", "disas main", and "x/8x $rbp" to see the distance between the return address and the base of the stack.)
test3.c is to try to simulate the attack shell code in a similar program to see whether it can succeed.
attack.c is the final solution attack of this challenge.

The meaning of each parameter during compilation：
-g	for debug.
-static	On systems that support dynamic linking, this overrides -pie and prevents linking with the shared libraries. On other systems, this option has no effect.
-no-pie	Don't produce a dynamically linked position independent executable.
-O0	No compile optimization.
-z execstack		Makes the binary code on the stack executable.
-fno-stack-protector	Cancel stack canary protection.

Buffer overflow protection and possible defacts:

Stack Canary: 
• A random value whose integrity will be checked before the return address. Buffer overflow attacks will likely modify the canary, so it will be found.
Attack:  
• The attacker can insert code to wisely outputs contents of the stack. So he can get the information of the stack without modifying the canary.
• So, the attacker can overwrite the return address without modifying the stack canary.

Bounds Checking:
• Use type safe memory language, or use fat pointer.
• The fat pointer additionally store the supposed length of the memory and check it at runtime.
Defect:
• This method can greatly improve the security of the program, but it will reduce the performance of the program since it has to be checked at runtime.

Tagging:
• Marking certain areas of memory as non-executable to prevent memory allocated to store data from containing executable code. 
• Also, certain areas of memory can be marked as non-allocated to prevent buffer overflows.
Attack:  
• In just-in-time (JIT) compilation, machine code is generated from some intermediate language (often called byte code) at run time,
   and the memory is generally exempted from protection. So the attacker can use JIT compilers to write malicious code.
• The attacker can use return-oriented programming. By choosing special return address, he can make the control flow go to an executable
   memory region (in some library) that is needed. This method can provide a small, useful functionality for the attacker (gadget). By combining
   calls to gadgets, the attacker can obtain a Turing complete language to do whatever he wants.

Address Space Layout Randomization (ASLR):
• When the program starts, arrange important memory areas randomly like stack, heap, code, and libraries. The attacker will be less likely to find 
   a good return address for the stack and libraries. The wrong guess attempts can cause the program crah and restart with a new randomization.
Attack:  
• The attacker can use long nop sequences in front of the malicious code to increase the possibility of execution (even it's not precise).
• The ASLR addresses can be leaked by side-channel attack on CPU branch target prediction buffer. Such a technique was proposed by Evtyushkin et al. in 2016.
   (https://ieeexplore.ieee.org/document/7783743 )

