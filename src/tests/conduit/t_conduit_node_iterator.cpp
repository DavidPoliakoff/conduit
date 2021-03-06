//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2014-2016, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-666778
// 
// All rights reserved.
// 
// This file is part of Conduit. 
// 
// For details, see: http://software.llnl.gov/conduit/.
// 
// Please also read conduit/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
///
/// file: conduit_node_iterator.cpp
///
//-----------------------------------------------------------------------------


#include "conduit.hpp"

#include <iostream>
#include "gtest/gtest.h"

using namespace conduit;

//-----------------------------------------------------------------------------
TEST(conduit_node_iterator, simple_1)
{

    uint32   a_val  = 10;
    uint32   b_val  = 20;

    Node n;
    n["a"] = a_val;
    n["b"] = b_val;

    EXPECT_EQ(n["a"].as_uint32(),a_val);
    EXPECT_EQ(n["b"].as_uint32(),b_val);

    std::cout << n.to_json();
    
    NodeIterator itr = n.children();
    Node itr_info;
    itr.info(itr_info);
    std::cout <<itr_info.to_json("conduit_json") << std::endl;
    
    index_t i = 0;
    while(itr.has_next())
    {
        Node &n = itr.next();
        
        if(i == 0)
        {
            EXPECT_EQ("a",itr.path());
            EXPECT_EQ(i,itr.index());
            EXPECT_EQ(a_val,n.as_uint32());
        }
        else if(i == 1)
        {
            EXPECT_EQ("b",itr.path());
            EXPECT_EQ(i,itr.index());
            EXPECT_EQ(b_val,n.as_uint32());
        }
        i++;
    }
    
    i = 0;
    while(itr.has_previous())
    {
        Node &n = itr.previous();
        
        if(i == 1)
        {
            EXPECT_EQ("a",itr.path());
            EXPECT_EQ(i,itr.index());
            EXPECT_EQ(a_val,n.as_uint32());
        }
        else if(i == 2)
        {
            EXPECT_EQ("b",itr.path());
            EXPECT_EQ(i,itr.index());
            EXPECT_EQ(b_val,n.as_uint32());
        }
        i++;
    }
}


//-----------------------------------------------------------------------------
TEST(conduit_node_iterator, empty)
{
    uint32   a_val  = 10;
    uint32   b_val  = 20;

    Node n;
    n["a"] = a_val;
    n["b"] = b_val;
    n["c"]; // empty


    NodeIterator itr(n.children());
    
    while(itr.has_next())
    {
        Node &n = itr.next();
        n.print();
    }
    
    itr = n["c"].children();
    EXPECT_FALSE(itr.has_next());
    
}


//-----------------------------------------------------------------------------
TEST(conduit_node_iterator, move_cursor)
{
    uint32   a_val  = 10;
    uint32   b_val  = 20;

    Node n;
    n["a"] = a_val;
    n["b"] = b_val;
    n["c"] = "myval";
    

    NodeIterator itr;
    
    itr = n.children();

    itr.to_back();
    EXPECT_FALSE(itr.has_next());
    
    // we are at the end,  check error capture
    EXPECT_THROW(itr.next(),conduit::Error);
    EXPECT_THROW(itr.peek_next(),conduit::Error);
    
    Node &last = itr.peek_previous();
    EXPECT_EQ(last.as_string(),"myval");
    
    itr.to_front();
    EXPECT_TRUE(itr.has_next());
    
    // we are at the beginning, check error capture
    EXPECT_THROW(itr.previous(),conduit::Error);
    EXPECT_THROW(itr.peek_previous(),conduit::Error);
    
    Node &first = itr.peek_next();
    EXPECT_EQ(first.as_uint32(),a_val);

    EXPECT_TRUE(itr.has_next());
    EXPECT_EQ(itr.next().as_uint32(),a_val);
    EXPECT_EQ(itr.path(),"a");
    
    EXPECT_TRUE(itr.has_next());
    EXPECT_EQ(itr.next().as_uint32(),b_val);
    EXPECT_EQ(itr.path(),"b");

    EXPECT_TRUE(itr.has_next());
    EXPECT_EQ(itr.next().as_string(),"myval");
    EXPECT_EQ(itr.path(),"c");
    
    EXPECT_FALSE(itr.has_next());

}

