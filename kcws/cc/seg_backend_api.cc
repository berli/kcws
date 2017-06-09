/*
 * Copyright 2016- 2018 Koth. All Rights Reserved.
 * =====================================================================================
 * Filename:  seg_backend_api.cc
 * Author:  Koth
 * Create Time: 2016-11-20 20:43:26
 * Description:
 *
 */
#include <string>
#include <thread>
#include <memory>
#include <fstream>

#include "tinyxml2.h"
#include "base/base.h"
#include "utils/jsonxx.h"
#include "utils/basic_string_util.h"
#include "kcws/cc/demo_html.h"
#include "kcws/cc/tf_seg_model.h"
#include "kcws/cc/pos_tagger.h"
#include "third_party/crow/include/crow.h"
#include "tensorflow/core/platform/init_main.h"

#define MAX_SEN_LEN 80
using namespace std;
using namespace tinyxml2;

DEFINE_int32(port, 9090, "the  api serving binding port");
DEFINE_string(model_path, "models/seg_model.pbtxt", "the model path");
DEFINE_string(vocab_path, "models/basic_vocab.txt", "char vocab path");
DEFINE_string(pos_model_path, "models/pos_model.pbtxt", "the pos tagging model path");
DEFINE_string(word_vocab_path, "models/word_vocab.txt", "word vocab path");
DEFINE_string(pos_vocab_path, "models/pos_vocab.txt", "pos vocab path");
DEFINE_string(config_path, "config.xml", "config param, such as listen port");
//DEFINE_int32(max_sentence_len, 80, "max sentence len ");
DEFINE_int32(max_sentence_len, MAX_SEN_LEN, "max sentence len ");
DEFINE_string(user_dict_path, "", "user dict path");
//DEFINE_int32(max_word_num, 50, "max num of word per sentence ");
DEFINE_int32(max_word_num, 50, "max num of word per sentence ");
class SegMiddleware {
 public:
  struct context {};
  SegMiddleware() {}
  ~SegMiddleware() {}
  void before_handle(crow::request& req, crow::response& res, context& ctx) {}
  void after_handle(crow::request& req, crow::response& res, context& ctx) {}
 private:
};

/*
<seg_backend_api>
<port>
</port>
</seg_backend_api>
*/
int getConfig()
{
    XMLDocument doc;
    doc.LoadFile( FLAGS_config_path.c_str() );
    if( doc.ErrorID() != 0)
    {
        return -1;
    }
    const char *port = doc.FirstChildElement("seg_backend_api")->FirstChildElement("port")->GetText();
    FLAGS_port = atoi(port);
    if( FLAGS_port == 0)
       FLAGS_port = 9090;
    LOG(INFO)<<"port:"<<FLAGS_port;

    return 0;
/*
    ifstream ifr(FLAGS_config_path.c_str());
    if(!ifr)
    {
       LOG(ERROR)<<"can't open "<<FLAGS_config_path;
       return -1;
    }
    string lsline;
    while(getline(ifr,lsline))
    {
       int liPos = lsline.find("port=");
       if(liPos != -1 )
       {
          FLAGS_port = atoi(lsline.substr(liPos + 5).c_str());
	  return 0;
       }
    }
    return -1;
*/
}

int main(int argc, char* argv[]) 
{
  tensorflow::port::InitMain(argv[0], &argc, &argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  crow::App<SegMiddleware> app;
  kcws::TfSegModel model;
  CHECK(model.LoadModel(FLAGS_model_path,
                        FLAGS_vocab_path,
                        FLAGS_max_sentence_len,
                        FLAGS_user_dict_path))
      << "Load model error";
  if (!FLAGS_pos_model_path.empty()) 
  {
    kcws::PosTagger* tagger = new kcws::PosTagger;
    CHECK(tagger->LoadModel(FLAGS_pos_model_path,
                            FLAGS_word_vocab_path,
                            FLAGS_vocab_path,
                            FLAGS_pos_vocab_path,
                            FLAGS_max_word_num)) << "load pos model error";
    model.SetPosTagger(tagger);
  }
  CROW_ROUTE(app, "/tf_seg/api").methods("POST"_method)
  ([&model](const crow::request & req) 
  {
    jsonxx::Object obj;
    int status = -1;
    std::string desc = "OK";
    std::string gotReqBody = req.body;
    LOG(INFO) << "got body:";
    LOG(INFO)<<gotReqBody.c_str();
    LOG(INFO) << "body len:"<<gotReqBody.size();

    jsonxx::Object toRet;
    if (gotReqBody.size() <= MAX_SEN_LEN && obj.parse(gotReqBody) && obj.has<std::string>("sentence")) 
    {
      std::string sentence = obj.get<std::string>("sentence");
      std::vector<std::string> result;
      std::vector<std::string> tags;
      if (model.Segment(sentence, &result, &tags)) 
      {
        status = 0;
        jsonxx::Array rarr;
        if (result.size() == tags.size()) 
        {
          int nl = result.size();
          for (int i = 0; i < nl; i++) 
          {
            jsonxx::Object obj;
            obj << "tok" << result[i];
            obj << "pos" << tags[i];
            rarr << obj;
          }
        } 
        else 
        {
          for (std::string str : result) 
          {
            rarr << str;
          }
        }
        toRet << "segments" << rarr;
      }//if(model.
    } 
    else 
    {
      if( gotReqBody.size() > MAX_SEN_LEN)
      {
          status = 1;
          desc = "sencent len: ";
	  desc += std::to_string(gotReqBody.size());
          desc += " is more than ";
	  desc += std::to_string(MAX_SEN_LEN);
      }
      else
      {
          status = -1;
          desc = "Parse request error";
      }
    }
    toRet << "status" << status;
    toRet << "msg" << desc;
    LOG(INFO)<<toRet.json();
    return crow::response(toRet.json());
  });

  CROW_ROUTE(app, "/")([](const crow::request & req) 
  {
    return crow::response(std::string(reinterpret_cast<char*>(&kcws_cc_demo_html[0]), kcws_cc_demo_html_len));
  });
  getConfig();
  app.port(FLAGS_port).multithreaded().run();
  return 0;
}

