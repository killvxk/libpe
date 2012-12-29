#include "stdafx.h"
#include "PE/PESectionHeader.h"

LIBPE_NAMESPACE_BEGIN

template <class T>
error_t
PESectionHeaderT<T>::GetSection(IPESectionT<T> **ppSection)
{
    if(NULL == m_pSection) {
        LIBPE_ASSERT_RET(NULL != m_pParser, ERR_FAIL);
        if(ERR_OK != m_pParser->ParseSection(GetRawStruct(), &m_pSection) || NULL == m_pSection) {
            return ERR_FAIL;
        }
    }

    return m_pSection.CopyTo(ppSection);
}

LIBPE_FORCE_TEMPLATE_REDUCTION_CLASS(PESectionHeaderT);

LIBPE_NAMESPACE_END