#ifndef __LIST_H__
#define __LIST_H__

#pragma once

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
