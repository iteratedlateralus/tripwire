/*
 * Simple Mail Submission Agent using SMTP with libEtPan!
 * TODO: Full sendmail like interface
 */

#include "sendmail.h"
static int smtp_tls = 0; 
static int smtp_esmtp = 0;
int smtp_send_message(
    char* smtp_server,
    uint16_t smtp_port,
    char* smtp_user,
    char* smtp_password,
    char* smtp_from,
    char *data, size_t len, char**rcpts) {
  int s = -1;
  int ret;
  char **r;
  int esmtp = 0;
  mailsmtp *smtp = NULL;

  if ((smtp = mailsmtp_new(0, NULL)) == NULL) {
    perror("mailsmtp_new");
    goto error;
  }

  /* first open the stream */
  if ((ret = mailsmtp_socket_connect(smtp, 
                     (smtp_server != NULL ? smtp_server : "localhost"),
                     smtp_port)) != MAILSMTP_NO_ERROR) {
    fprintf(stderr, "mailsmtp_socket_connect: %s\n", mailsmtp_strerror(ret));
    goto error;
  }
  
  /* then introduce ourselves */
  if (smtp_esmtp && (ret = mailesmtp_ehlo(smtp)) == MAILSMTP_NO_ERROR)
    esmtp = 1;
  else if (!smtp_esmtp || ret == MAILSMTP_ERROR_NOT_IMPLEMENTED)
    ret = mailsmtp_helo(smtp);
  if (ret != MAILSMTP_NO_ERROR) {
    fprintf(stderr, "mailsmtp_helo: %s\n", mailsmtp_strerror(ret));
    goto error;
  }

  if (esmtp && smtp_tls &&
      (ret = mailsmtp_socket_starttls(smtp)) != MAILSMTP_NO_ERROR) {
    fprintf(stderr, "mailsmtp_starttls: %s\n", mailsmtp_strerror(ret));
    goto error;
  }
  
  if (esmtp && smtp_tls) {
    /* introduce ourselves again */
    if (smtp_esmtp && (ret = mailesmtp_ehlo(smtp)) == MAILSMTP_NO_ERROR)
      esmtp = 1;
    else if (!smtp_esmtp || ret == MAILSMTP_ERROR_NOT_IMPLEMENTED)
      ret = mailsmtp_helo(smtp);
    if (ret != MAILSMTP_NO_ERROR) {
      fprintf(stderr, "mailsmtp_helo: %s\n", mailsmtp_strerror(ret));
      goto error;
    }
  }
  
  if (esmtp && smtp_user != NULL &&
      (ret = mailsmtp_auth(smtp, smtp_user,
               (smtp_password != NULL) ? smtp_password : ""))
      != MAILSMTP_NO_ERROR) {
    fprintf(stderr, "mailsmtp_auth: %s: %s\n", smtp_user, mailsmtp_strerror(ret));
    goto error;
  }

  /* source */
  if ((ret = (esmtp ?
          mailesmtp_mail(smtp, smtp_from, 1, "etPanSMTPTest") :
          mailsmtp_mail(smtp, smtp_from))) != MAILSMTP_NO_ERROR) {
    fprintf(stderr, "mailsmtp_mail: %s, %s\n", smtp_from, mailsmtp_strerror(ret));
    goto error;
  }
  
  /* recipients */
  for (r = rcpts; *r != NULL; r++) {
    if ((ret = (esmtp ?
        mailesmtp_rcpt(smtp, *r,
                   MAILSMTP_DSN_NOTIFY_FAILURE|MAILSMTP_DSN_NOTIFY_DELAY,
                   NULL) :
        mailsmtp_rcpt(smtp, *r))) != MAILSMTP_NO_ERROR) {
      fprintf(stderr, "mailsmtp_rcpt: %s: %s\n", *r, mailsmtp_strerror(ret));
      goto error;
    }
  }
  
  /* message */
  if ((ret = mailsmtp_data(smtp)) != MAILSMTP_NO_ERROR) {
    fprintf(stderr, "mailsmtp_data: %s\n", mailsmtp_strerror(ret));
    goto error;
  }
  if ((ret = mailsmtp_data_message(smtp, data, len)) != MAILSMTP_NO_ERROR) {
    fprintf(stderr, "mailsmtp_data_message: %s\n", mailsmtp_strerror(ret));
    goto error;
  }
  mailsmtp_free(smtp);
  return 0;

 error:
  if (smtp != NULL)
    mailsmtp_free(smtp);
  if (s >= 0)
    close(s);
  return -1;
}

int build_smtp_message(char* file,char* message,int max_len){
    FILE* fp = NULL;

    strcpy(message,"Tripwire Alert\n");
    fp = fopen(file,"r");
    if(!fp){
        fprintf(stderr,"Unable to open template file\n");
        return -1;
    }
    if( fread(&message[16],sizeof(char),max_len - 16,fp) == 0 ){
        fprintf(stderr,"Error reading from template file\n");
        return -2;
    }
    return 0;
}
