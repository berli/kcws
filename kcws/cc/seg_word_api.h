
/*
 * Copyright 2016- 2018 Koth. All Rights Reserved.
 * =====================================================================================
 * Filename:  seg_word_api.cc
 * Author:  berli
 * Create Time: 2017-05-31 20:43:26
 * Description:
 *
 */
#ifndef SEG_WORD_API_H_H_
#define SEG_WORD_API_H_H_

#include <string>
#include <thread>
#include <memory>

#include "kcws/cc/tf_seg_model.h"
#include "kcws/cc/pos_tagger.h"

using namespace std;

struct segInfo
{
   string pos;
   string tok;
};

struct wordsInfo
{
   string sentence;
   vector<segInfo>vecSeg;
};

class SegWord
{
public:
    SegWord();
    ~SegWord();

    int initModel(const string&asWordVocab, const string&asPosModelPath,const string&asPosVocabpath, const int&aiMaxWordNum, const string&asModelPath, const string&asVocabPath,const int&aiMaxSentenceLen, const string&asUserDictPath=NULL);
 
    int eval(const vector<string>&aVecSentence, vector<wordsInfo>&aVecSeg);  

protected:
    kcws::TfSegModel coModel;
    kcws::PosTagger* cpTagger;
};

#endif

