#ifndef QCONVERSIONS_H
#define QCONVERSIONS_H

#include "../LDPC-codes/rand.h"
#include "../LDPC-codes/alloc.h"
#include "../LDPC-codes/intio.h"
#include "../LDPC-codes/open.h"
#include "../LDPC-codes/mod2sparse.h"
#include "../LDPC-codes/mod2dense.h"
#include "../LDPC-codes/mod2convert.h"
#include "../LDPC-codes/rcode.h"
#include "../LDPC-codes/distrib.h"

#include <QVector>

class LDPCQConversions
{
public:
    typedef enum { Sparse, Dense, Mixed } generator_type;
    LDPCQConversions();

    static bool write_qvector_to_file(QString filename,QVector<int> &from);
    static bool write_qvector_to_file_old_format(QString filename,QVector<int> &from);

    static bool read_qvector_to_file(QString filename,QVector<int> &to);
    static bool read_qvector_to_file_old_format(QString filename,QVector<int> &to);

    bool mod2sparse_append_to_qvector(QVector<int> &to);
    bool mod2sparse_append_to_qvector(QVector<int> &to,mod2sparse *m);
    bool mod2sparse_write_to_qvector(QVector<int> &to,mod2sparse *m);
    mod2sparse *mod2sparse_read_from_qvector(QVector<int> &from);
    mod2sparse *mod2sparse_read_from_qvector(QVector<int> &from,int &offset);

    bool mod2sparse_save(QVector<int> &to);
    bool mod2sparse_load(QVector<int> &from);

    bool mod2sparse_save_parity_check_to_qvector(QVector<int> &to);
    bool mod2sparse_load_parity_check_from_qvector(QVector<int> &from);


    bool save_generator_to_qvector(QVector<int> &to,generator_type gentype);
    bool load_generator_from_qvector(QVector<int> &from,int cols_only,int no_pchk_file);

    bool mod2dense_append_to_qvector(QVector<int> &to,mod2dense *m);
    mod2dense *mod2dense_read_from_qvector(QVector<int> &from,int &offset);

};

#endif // QCONVERSIONS_H
