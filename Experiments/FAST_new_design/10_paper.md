### abstract
SGX uses EPC Enclave Page Cache to provide confidentiality, integrity and authentication.
However the current design of SGX operates in limited capacity in order to reduce the page swap between EPC and non-EPC regions.

In order to solve the page overhead problem
EPC size should be increased so that it matches the physcal memory.

However EPC size should take care of memory bandwidth overhead check and space overheads.
The two problems, memory bandwdith and overhead check are solved by introducing the VAULT.
VALUT provides
1. compact and lower depth to defcrease the memory bandwidth overhead check
2. Mac sharing and compression that decreases the space overheads

memory capapcity overhead in only 4.7%
the overall performance is 3.7x
