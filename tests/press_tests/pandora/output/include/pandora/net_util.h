/*
 * @landyliu
*/

#ifndef PANDORA_UTIL_NET_UTIL_H_ 
#define PANDORA_UTIL_NET_UTIL_H_    1 

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/time.h>
#include <net/if.h>
#include <inttypes.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <string>

namespace pandora 
{

// ip address conversion
class NetUtil 
{
public:
    // get local machine ip 
    static uint32_t GetLocalAddr(const char *dev_name);
    // is local machine ip, true - yes, false - no
    static bool IsLocalAddr(uint32_t ip, bool loopSkip = true);
    // convert string-format ip to int. E.g,  10.0.100.89 => 1499725834
    static uint32_t GetAddr(const char *ip);
    // convert uint64 to string
    static std::string AddrToString(uint64_t ipport);
    // convert ip : port to unit64
    static uint64_t StrToAddr(const char *ip, int port);
    static uint64_t IpToAddr(uint32_t ip, int port);

private:
    NetUtil();
    ~NetUtil();
};

} // end of namespace

#endif
