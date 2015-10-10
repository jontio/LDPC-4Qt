#LDPC-4Qt

###LDPCCode class

Contains the LDPC code. It has the ability to randomly create an LDPC code as well as being able to save and load LDPC codes from disk.

Both a Parity Check Matrix and a Generator Matrix are needed for LDPC code use. The Parity Check Matrix is used for decoding while the Generator Matrix is used for encoding. These two matrices are stored in the LDPCCode class as public members of type ```QVector<int>```. The LDPCQConversions class deals with converting matrices of this type into a form that LDPC-codes can read.

The creation of LDPC codes are via the following public member function.

```C++
bool create(int M, int N, int seed, LDPCMakeLDPC::make_method parity_check_make_method, QString distrib, int no4cycle, LDPCMakeGen::make_method generator_make_method=LDPCMakeGen::Dense, mod2sparse_strategy strategy = Mod2sparse_minprod, int abandon_number=0, int abandon_when=0);
```

This function deals with the creation of the parity check matrix and the generator matrix. Everything up to and including ```no4cycle``` is for the parity check matrix while the rest is for the generator matrix. This function roughly combines the functionality of [make-ldpc](http://radfordneal.github.io/LDPC-codes/pchk.html#make-ldpc) and [make-gen](http://radfordneal.github.io/LDPC-codes/encoding.html#make-gen) of LDPC-codes.

* ```M``` and ```N``` specify the size of the parity check matrix. This means the code with N-M message bits in and N code bits out.
* ```seed``` is the random number generation seed.
* ```parity_check_make_method``` is either ```LDPCMakeLDPC::Evencol``` or ```LDPCMakeLDPC::Evenboth``` (see [make-ldpc](http://radfordneal.github.io/LDPC-codes/pchk.html#make-ldpc) for meaning)
* ```distrib``` is a QString of either checks-per-col or checks-distribution (see [make-ldpc](http://radfordneal.github.io/LDPC-codes/pchk.html#make-ldpc) for meaning)
* ```no4cycle``` will cause cycles of length four in the factor graph representation of the code to be eliminated if possible
* ```generator_make_method``` is ```LDPCMakeGen::Sparse```, ```LDPCMakeGen::Dense```, ```LDPCMakeGen::Mixed``` and will create a generation matrix using this method. Sparse generator matrices are slow to create but use less memory while Dense generator matrices a fast create and use more memory.
* ``strategy``, ```abandon_number``` and ```abandon_when``` Are only used for sparse generation matrix creation (see [make-gen](http://radfordneal.github.io/LDPC-codes/encoding.html#make-gen) for details of meaning)

The following public member functions are for saving and loading LDPC codes to disk

```C++
bool savetofile(QString filename, file_format fileformat=NewFormat1);
bool loadfromfile(QString filename, file_format fileformat=NewFormat1);
```

* ```filename``` is the filename
* ```fileformat``` is the disk format and is either ```LDPCCode::OldFormat``` or ```LDPCCode::NewFormat1```. Only reading ```LDPCCode::NewFormat1``` is supported. ```LDPCCode::OldFormat``` is compatible with files created using of LDPC-codes.

The LDPCCode class contains the following miscellaneous public member functions.

```C++
bool isEmpty();
bool setRandFileName(QString filename);
```

The first one tests if the class is storing an LDPC code while the second one will set the file name used by the random number generator in rand.c which is used for some tasks.

The LDPCCode class has the following three constructors.

```C++
LDPCCode();
LDPCCode(QString filename);
LDPCCode(int M, int N, int seed, LDPCMakeLDPC::make_method parity_check_make_method, QString distrib, int no4cycle, LDPCMakeGen::make_method generator_make_method=LDPCMakeGen::Dense, mod2sparse_strategy strategy = Mod2sparse_minprod, int abandon_number=0, int abandon_when=0);
```

* ```LDPCCode();``` creates an empty parity check matrix and generator matrix.
* ```LDPCCode(QString filename);``` does the same as the first but sets the file name used by the random number generator in rand.c
* ```LDPCCode(int M, int N, int seed, LDPCMakeLDPC::make_method parity_check_make_method, QString distrib, int no4cycle, LDPCMakeGen::make_method generator_make_method=LDPCMakeGen::Dense, mod2sparse_strategy strategy = Mod2sparse_minprod, int abandon_number=0, int abandon_when=0);``` creates a random LDPC code using the ```create``` method.

The following shows an example to create an LDPC code of 1000 bit long messages and 2000 bit long code words.

```C++
LDPCCode ldpccode(1000,2000,2,LDPCMakeLDPC::Evenboth,"3",true);
```

