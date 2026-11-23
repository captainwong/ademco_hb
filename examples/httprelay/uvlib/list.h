#ifndef __LIST_H__
#define __LIST_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define list_append(type, l, node)   \
    do {                             \
        type* tail_ = (l);           \
        while (tail_ && tail_->next) \
            tail_ = tail_->next;     \
        if (tail_)                   \
            tail_->next = (node);    \
        else                         \
            (l) = node;              \
    } while (0);

#endif
