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



### Introudction

Cloud systems protect from malicious OS and physical access from cloud operators
In order to prvent the attacks cloud systems adapt CIA
C - confidentiality: encrypt signals from process
I - Integrity. memory system retruns the last written block
    This place is knowned to be the problematic because it needs to navigate the tree-based struct
A - Authentity - HW enforced permission checks

Using SGX is one way of applying to CIA
SGX uses enclave that separates EPC that stores sensitive pages
non-EPC that stores spilled out sensitive pages.

MEE Controller provides permission checks, encryption/decryption, integerity of the tree.

The problem is marking all pages as sensitive creates large overhead
The Figure 1 explains the overhead created by EPC miss, Cost of moving between EPC to nonEPC and EPC hit.
The Figure expalins the overheads that are caused by sensitive pages using both Eloes and SCONE.


The solution is to increase the size of SGX so that it will cover the physical memory.
However large depth and size will cause
1. poor cacheability
2. higher bandwidth navigation
3. memory capacity overhead.
4. increased workload demands

The paper introduces VAULT
that enlarges the EPC region but
1. lowers the bandwidth
2. decrease the capacity overhead
3. disable overhead when non-sensitive blocks are part of integrity tree.

The 3 advantages are implemented by
1. designing the variable arity from 16 to 64.
2. flatten the tree and make the cacheability more compact
3. propose a technique SMC that uses compression to pack a ddata block and its MAC into a single cacheline.


### Background
















**10 pages**
