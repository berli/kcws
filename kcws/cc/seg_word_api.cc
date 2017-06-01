/*
 * Copyright 2016- 2018 Koth. All Rights Reserved.
 * =====================================================================================
 * Filename:  seg_word_api.cc
 * Author:  berli
 * Create Time: 2017-05-31 20:43:26
 * Description:
 *
 */

#include <iostream>
#include "seg_word_api.h"

SegWord::SegWord()
{
   cpTagger = NULL;
}

SegWord::~SegWord()
{ 
}

int SegWord::initModel(const string&asWordVocab, const string&asPosModelPath,const string&asPosVocabpath, const int&aiMaxWordNum, const string&asModelPath, const string&asVocabPath,const int&aiMaxSentenceLen, const string&asUserDictPath)
{
   LOG(INFO)<<"LoadModel ..."<<endl;
   bool lbRet = coModel.LoadModel(asModelPath,
                        asVocabPath,
                        aiMaxSentenceLen,
                        asUserDictPath);
  if(!lbRet)
  {
    LOG(INFO)<<"LoadModel failed"<<endl;
    return -1;
  }
  LOG(INFO)<<"LoadModel OK..."<<endl;

  if (!asPosModelPath.empty()) 
  {
    if(cpTagger)
       delete cpTagger;

    cpTagger = new kcws::PosTagger;
    LOG(INFO)<<"Load pos model ..."<<endl;
    lbRet = cpTagger->LoadModel(asPosModelPath,
                            asWordVocab,
                            asVocabPath,
                            asPosVocabpath,
                            aiMaxWordNum);
    coModel.SetPosTagger(cpTagger);
  }
  else
  {
     LOG(INFO)<<"LoadModel OK..."<<endl;
     return -2;
  }

   return 0;
}

int SegWord::eval(const vector<string>&aVecSentence, vector<wordsInfo>&aVecSeg)
{
  LOG(INFO)<<"eval..."<<endl;
  for(size_t i = 0; i < aVecSentence.size(); i++)
  {
      std::vector<std::string> result;
      std::vector<std::string> tags;
      if (coModel.Segment(aVecSentence[i], &result, &tags)) 
      {
        LOG(INFO)<<aVecSentence[i]<<" result size:"<<result.size()<<" tags size:"<<tags.size()<<endl;
        if (result.size() == tags.size()) 
        {
          int nl = result.size();
          wordsInfo words;
          words.sentence = aVecSentence[i];
          for (int j = 0; j < nl; j++) 
          { 
            segInfo lseg;
            lseg.pos = tags[j];
            lseg.tok = result[j];
            words.vecSeg.push_back(lseg);
           
            LOG(INFO)<<"pos:"<<lseg.pos<<" tok:"<<lseg.tok<<endl;
          }//for
          aVecSeg.push_back(words);
        }//if
      }//if
      else
      {
         return -1;
      }
  }//for
  
  return 0;
}

