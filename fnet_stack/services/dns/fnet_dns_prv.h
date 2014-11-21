/**************************************************************************
* 
* Copyright 2014 by Andrey Butok. FNET Community.
*
***************************************************************************
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License Version 3 
* or later (the "LGPL").
*
* As a special exception, the copyright holders of the FNET project give you
* permission to link the FNET sources with independent modules to produce an
* executable, regardless of the license terms of these independent modules,
* and to copy and distribute the resulting executable under terms of your 
* choice, provided that you also meet, for each linked independent module,
* the terms and conditions of the license of that module.
* An independent module is a module which is not derived from or based 
* on this library. 
* If you modify the FNET sources, you may extend this exception 
* to your version of the FNET sources, but you are not obligated 
* to do so. If you do not wish to do so, delete this
* exception statement from your version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* You should have received a copy of the GNU General Public License
* and the GNU Lesser General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*
**********************************************************************/ /*!
*
* @file fnet_dns_prv.h
*
* @author Andrey Butok
*
* @brief DNS Definitions.
*
***************************************************************************/

#ifndef _FNET_DNS_PRV_H_

#define _FNET_DNS_PRV_H_

/************************************************************************
*     Definitions
*************************************************************************/
/* Size limits. */
#define FNET_DNS_MAME_SIZE      (255)     /* RFC1035:To simplify implementations, the total length of a domain name (i.e.,
                                           * label octets and label length octets) is restricted to 255 octets or less.*/
#define FNET_DNS_MESSAGE_SIZE   (512)     /* Messages carried by UDP are restricted to 512 bytes (not counting the IP
                                           * or UDP headers).  
                                           * Longer messages (not supported) are truncated and the TC bit is set in
                                           * the header.*/    

/********************************************************************/ /*!
* @brief DNS Resorce Record Types 
*************************************************************************/
#define    FNET_DNS_TYPE_A      (0x0001)   /**< @brief IPv4 address. */
#define    FNET_DNS_TYPE_AAAA   (0x001C)    /**< @brief IPv6 address. */


/************************************************************************
*    DNS header [RFC1035, 4.1.1.]
*************************************************************************
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/  
    
FNET_COMP_PACKED_BEGIN
typedef struct
{
    unsigned short id FNET_COMP_PACKED;         /* A 16 bit identifier assigned by the program that
                                                * generates any kind of query. This identifier is copied
                                                * the corresponding reply and can be used by the requester
                                                * to match up replies to outstanding queries. */
    unsigned short flags FNET_COMP_PACKED;      /* Flags.*/
    unsigned short qdcount FNET_COMP_PACKED;    /* An unsigned 16 bit integer specifying the number of
                                                * entries in the question section.*/
    unsigned short ancount FNET_COMP_PACKED;    /* An unsigned 16 bit integer specifying the number of
                                                * resource records in the answer section.*/
    unsigned short nscount FNET_COMP_PACKED;    /* an unsigned 16 bit integer specifying the number of name
                                                * server resource records in the authority records
                                                * section.*/
    unsigned short arcount FNET_COMP_PACKED;    /* An unsigned 16 bit integer specifying the number of
                                                * resource records in the additional records section.*/

} fnet_dns_header_t;
FNET_COMP_PACKED_END

#define FNET_DNS_HEADER_FLAGS_QR    (0x8000) /* Query (0), Response (1)*/
#define FNET_DNS_HEADER_FLAGS_AA    (0x0400) /* Authoritative Answer. */
#define FNET_DNS_HEADER_FLAGS_TC    (0x0200) /* TrunCation. */
#define FNET_DNS_HEADER_FLAGS_RD    (0x0100) /* Recursion Desired. */
#define FNET_DNS_HEADER_FLAGS_RA    (0x0080) /* Recursion Available. */

/************************************************************************
*    DNS Question section [RFC1035, 4.1.2.]
*************************************************************************
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                     QNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QTYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QCLASS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/  
    
FNET_COMP_PACKED_BEGIN
typedef struct
{
    unsigned char   zero_length FNET_COMP_PACKED;   /* The domain name terminates with the
                                                    * zero length octet for the null label of the root. */
    unsigned short  qtype FNET_COMP_PACKED;         /* Specifies the type of the query.*/
    unsigned short  qclass FNET_COMP_PACKED;        /* Specifies the class of the query.*/

} fnet_dns_q_tail_t;
FNET_COMP_PACKED_END


/************************************************************************
*    DNS Resource Record header [RFC1035, 4.1.3.] with message compression
*************************************************************************
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                                               /
    /                      NAME                     /
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     CLASS                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TTL                      |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                   RDLENGTH                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
    /                     RDATA                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/  
    
FNET_COMP_PACKED_BEGIN
typedef struct
{
    union {
        unsigned char   name_ptr_c[2] FNET_COMP_PACKED; /* A domain name to which this resource record pertains.
                                                        * For compression, it is replaced with a pointer to a prior occurance
                                                        * of the same name */
        unsigned short  name_ptr FNET_COMP_PACKED;
    };
    unsigned short  type FNET_COMP_PACKED;          /* This field specifies the meaning of the data in the RDATA
                                                    * field.*/
    unsigned short  rr_class FNET_COMP_PACKED;     /* An unsigned 16 bit integer specifying the number of
                                                    * entries in the question section.*/
    unsigned long   ttl FNET_COMP_PACKED;           /* Specifies the time interval (in seconds) that the 
                                                    * resource record may be
                                                    * cached before it should be discarded.*/
    unsigned short  rdlength FNET_COMP_PACKED;      /* Length in octets of the RDATA field.*/
    unsigned long   rdata FNET_COMP_PACKED;         /* The format of this information varies
                                                    * according to the TYPE and CLASS of the resource record. 
                                                    * If the TYPE is A and the CLASS is IN,
                                                    * the RDATA field is a 4 octet ARPA Internet address.*/

} fnet_dns_rr_header_t;
FNET_COMP_PACKED_END

/* [RFC1035 4.1.4.] In order to reduce the size of messages, the domain system utilizes a
* compression scheme which eliminates the repetition of domain names in a
* message. In this scheme, an entire domain name or a list of labels at
* the end of a domain name is replaced with a pointer to a prior occurance
* of the same name.
* The pointer takes the form of a two octet sequence:
* +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
* | 1  1|                OFFSET                   |
* +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/
#define     FNET_DNS_NAME_COMPRESSED_MASK           (0xC0)       /**< @brief Mark that name is compressed. */
#define     FNET_DNS_NAME_COMPRESSED_INDEX_MASK     (0x3FFF)     /**< @brief Index of compressed name. */

#define FNET_DNS_HEADER_CLASS_IN    (0x01)  /* The Internet.*/

#endif /* _FNET_DNS_PRV_H_*/
