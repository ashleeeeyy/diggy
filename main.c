/*
 * DNS stuff lol
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "lib/buffer_utils/bytebuf.h"

struct dns_header {
    //first int, the ID
    u_int16_t   id;

    //second int, flags
    u_int16_t   QR : 1,
                OPCODE : 4,
                AA : 1,
                TC : 1,
                RD : 1,
                RA : 1,
                Z : 3,
                RCODE : 4;

    u_int16_t qdcount;
    u_int16_t ancount;
    u_int16_t nscount;
    u_int16_t arcount;
};

struct dns_question_footer {
    u_int16_t qtype;
    u_int16_t qclass;
};

int dns_name_split(char *src, const char delim){
    //  result which is the amount of times we split the str
    int times_split = 0;

    char *start = src;
    while(*src != '\0'){
        if(*src == delim){
            *src = '\0';
            times_split++;
        }
        ++src;
    }

    src = start;

    return times_split;

}

int buffer_write_dns_name(char *name, char buffer[], int *writehead, int *buffersize){
    char *input_domain_ptr = name;

    int ts = dns_name_split(input_domain_ptr, '.');

    for (int i = 0; i < ts+1; ++i){
        int pre_writehead = *writehead;
        buffer_write_string(input_domain_ptr, buffer, writehead, buffersize);
        input_domain_ptr += *writehead-pre_writehead;
    }
}

int main() {
    int writehead = 0;
    int socky = socket(AF_INET, SOCK_DGRAM, 0);
    if (socky<0){
        printf("could not open socket!\n");
        return -1;
    }

    //server address struct
    struct sockaddr_in server_address;
    //theres currently random stuff in there so lets give it some zeros
    memset(&server_address, 0, sizeof server_address);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("198.41.0.4");
    server_address.sin_port = htons(53); //short but in network form!

    //prepare our message to get shipped
    char queryDomain[11] = {'s', 't', 'a', 't', 'u', 't' , 'o' , ',', 'o', 'r', 'g'};

    char *buffer; //big buffer cuz we might need it idk
    int buffersize = 16;
    buffer = calloc(1, buffersize);

    struct dns_header header;
    memset(&header, 0, sizeof header);
    header.id = htons(1337);
    header.qdcount = htons(1);

    buffer_write_char_array((char *) &header, sizeof header, buffer, &writehead, &buffersize);

    char name[] = "a.statuto.org.";
    buffer_write_dns_name(name, buffer, &writehead, &buffersize);

    struct dns_question_footer questionFooter;
    questionFooter.qclass = htons(1);
    questionFooter.qtype = htons(1);

    buffer_write_u_int_16(questionFooter.qclass,
                          buffer,
                          &writehead,
                          &buffersize);
    buffer_write_u_int_16(questionFooter.qtype,
                          buffer,
                          &writehead,
                          &buffersize);


    debug_output_buffer(buffer, writehead);


    if (sendto(socky, buffer, writehead, 0, // +1 to include terminator
               (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("cannot send message");
        close(socky);
        return 1;
    }
    close(socky);




    return 0;
}
