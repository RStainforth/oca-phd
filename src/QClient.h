#ifndef __Q_CLIENT_H
#define __Q_CLIENT_H

#include <TSocket.h>
#include <TMessage.h>
#include <TObject.h>
#include <TString.h>
#include <TClass.h>

class QClient : public TObject
{
 public:
  QClient(Int_t port, const char *server = "localhost");
  ~QClient();
 
  void SetServer(const char *server){ fServer = server; }
  Int_t OpenServer(const char *option = "xmit");
  Int_t CloseServer();
  Int_t PingServer();
  ClassDef(QClient,0) //Base client class

 protected:
  TSocket  *fSocket;       //Socket 
  TMessage *fSendMessage;  //Message container
  TMessage *fRecvMessage;  //Message container
  TString   fServer;       //QServer hostname
  Int_t     fPort;         //Server port
};

#endif












