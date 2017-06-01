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
}

SegWord::~SegWord()
{ 
   if(cpTagger)
      delete cpTagger;
      
}

int SegWord::initModel(const string&asWordVocab, const string&asPosModelPath,const string&asPosVocabpath, const int&aiMaxWordNum, const string&asModelPath, const string&asVocabPath,const int&aiMaxSentenceLen, const string&asUserDictPath)
{
   cout<<"LoadModel ..."<<endl;
   bool lbRet = coModel.LoadModel(asModelPath,
                        asVocabPath,
                        aiMaxSentenceLen,
                        asUserDictPath);
  if(!lbRet)
  {
    cout<<"LoadModel failed"<<endl;
    return -1;
  }
  cout<<"LoadModel OK..."<<endl;

  if (!asPosModelPath.empty()) 
  {
    if(cpTagger)
       delete cpTagger;

    cpTagger = new kcws::PosTagger;
    cout<<"Load pos model ..."<<endl;
    lbRet = cpTagger->LoadModel(asPosModelPath,
                            asWordVocab,
                            asVocabPath,
                            asPosVocabpath,
                            aiMaxWordNum);
    coModel.SetPosTagger(cpTagger);
  }
  else
  {
     cout<<"LoadModel OK..."<<endl;
     return -2;
  }

   return 0;
}

int SegWord::eval(const vector<string>&aVecSentence, vector<wordsInfo>&aVecSeg)
{
  for(size_t i = 0; i < aVecSentence.size(); i++)
  {
      std::vector<std::string> result;
      std::vector<std::string> tags;
      if (coModel.Segment(aVecSentence[i], &result, &tags)) 
      {
        if (result.size() == tags.size()) 
        {
          int nl = result.size();
          for (int i = 0; i < nl; i++) 
          { 
            wordsInfo words;
            words.sentence = aVecSentence[i];
            words.pos = tags[i];
            words.tok = result[i];
            aVecSeg.push_back(words);
          }//for
        }//if
      }//if
      else
      {
         return -1;
      }
  }//for
  
  return 0;
}

