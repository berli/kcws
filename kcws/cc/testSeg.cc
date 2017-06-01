
#include <iostream>
#include "seg_word_api.h"

int main(int argc, char* argv[])
{
   SegWord loSeg;
   string asWordVocab ="kcws/models/word_vocab.txt"; 
   string asPosModelPath = "kcws/models/pos_model.pbtxt";
   string asPosVocabpath = "kcws/models/pos_vocab.txt";
   int    aiMaxWordNum = 50;
   string asModelPath = "kcws/models/seg_model.pbtxt"; 
   string asVocabPath = "kcws/models/basic_vocab.txt";
   int aiMaxSentenceLen = 80; 
   string asUserDictPath = "";

   int liRet  = loSeg.initModel(asWordVocab, asPosModelPath,asPosVocabpath, aiMaxWordNum, asModelPath, asVocabPath,aiMaxSentenceLen, asUserDictPath);
   LOG(INFO)<<"initModel:"<<liRet<<endl;
 
   vector<string>lVecSentence;
   for(int i =1; i <argc; i++)
   { 
     LOG(INFO)<<argv[i]<<endl;
     lVecSentence.push_back(argv[i]);
   }

   vector<wordsInfo>lVecSeg;
   liRet = loSeg.eval(lVecSentence, lVecSeg);
   for(size_t i = 0; i < lVecSeg.size(); i++)
   {
      LOG(INFO)<<lVecSeg[i].sentence;
      for(size_t j =0; j < lVecSeg[i].vecSeg.size();j++)
          LOG(INFO)<<" tok="<<lVecSeg[i].vecSeg[j].tok<<" pos="<<lVecSeg[i].vecSeg[j].pos<<endl;
   }
}
