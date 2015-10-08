#ifndef LDPC4QT_H
#define LDPC4QT_H

#include <stdio.h>
#include <stdlib.h>


extern "C" {
#include "../LDPC-codes/rand.h"
#include "../LDPC-codes/mod2sparse.h"
#include "../LDPC-codes/mod2dense.h"
#include "../LDPC-codes/channel.h"
#include "../LDPC-codes/dec.h"
#include "../LDPC-codes/enc.h"
#include "../LDPC-codes/alloc.h"
#include "../LDPC-codes/blockio.h"
#include "../LDPC-codes/intio.h"
#include "../LDPC-codes/check.h"
#include "../LDPC-codes/open.h"
#include "../LDPC-codes/mod2convert.h"
#include "../LDPC-codes/distrib.h"
#include "../LDPC-codes/rand.h"
#include "../LDPC-codes/rcode.h"
}


#include "ldpcencode.h"
#include "ldpcdecode.h"
#include "ldpcchannel.h"
#include "ldpcdecodingmethod.h"
#include "ldpcverify.h"
#include "ldpcmakeldpc.h"
#include "ldpcmakegen.h"
#include "ldpctransmitsimulation.h"
#include "ldpcqconversions.h"
#include "ldpccode.h"
#include "rcodevars.h"


#endif // LDPC4QT_H
