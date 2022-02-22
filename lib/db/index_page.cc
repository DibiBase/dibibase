#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "db/index_page.hh"

using namespace dibibase::db;

IndexPage::IndexPage():m_size(4096)  {}

static std::unique_ptr<IndexPage> IndexPage::from(util::Buffer *b) {
    auto d = Data::from(b)
    off_t f = b->get_uint64()
    m_sort_keys[d] = f ;
}

bool IndexPage::add_sort_key(catalog::Data *d , off_t f){
    int upcoming_size = sizeof(f) + d.m_size ;
    if(m_size-upcoming_size <= 0) return false;
    
    m_size-= upcoming_size;
    m_sort_keys[d] = f ;
    return true;
}

off_t IndexPage::find_key_offset(catalog::Data *d) {
    return m_sort_keys[d];
}

void IndexPage::bytes(util::Buffer *b) const {
    for(auto elm: m_sort_keys){
        elm.first->bytes(b); // data
        b->put_uint64(elm.second); // offset
    }
    // IDK how to pad remaining bytes
}