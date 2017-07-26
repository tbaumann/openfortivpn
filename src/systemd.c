
#include <systemd/sd-bus.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "systemd.h"
#include "tunnel.h"
#include "log.h"


void open_bus_connection(struct tunnel *tunnel)
{
  // Open connection if not open yet
  if ( tunnel->systemd.bus == NULL )
  {
    sd_bus *bus = NULL;
    int r;

    /* Connect to the system bus */
    r = sd_bus_open_system(&bus);
    if (r < 0) {
      log_error("Failed to connect to system bus: %s\n", strerror(-r));
      bus = NULL;
    }
    tunnel->systemd.bus = bus;
  }
}



int systemd_add_nameservers(struct tunnel *tunnel)
{
  sd_bus *bus = NULL;
  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus_message *m = NULL;
  sd_bus_message *reply = NULL;
  int r;
  struct dns_address {
    int32_t sin_family;
    struct in_addr ip_addr;
  };
  struct dns_address addresses[2];
  int addresses_size = 0;

	if (tunnel->ipv4.ns1_addr.s_addr != 0)
  {
    addresses[addresses_size].sin_family = AF_INET;
    addresses[addresses_size].ip_addr = tunnel->ipv4.ns1_addr;
    addresses_size++;
  }

	if (tunnel->ipv4.ns2_addr.s_addr != 0)
  {
    addresses[addresses_size].sin_family = AF_INET;
    addresses[addresses_size].ip_addr = tunnel->ipv4.ns2_addr;
    addresses_size++;
  }

  if ( addresses_size == 0 )
  {
	   log_info("No nameservers. Not updating systemd-resolve.\n");
     return 0;
  }

	log_info("Updating systemd-resolve...\n");

  open_bus_connection(tunnel);
  bus = tunnel->systemd.bus;

  if ( bus == NULL) {
    log_error("Can not open dbus\n");
    return 1;
  }

  r = sd_bus_message_new_method_call(
    bus, &m,
    BUS_SERVICE, BUS_OBJECT, BUS_INTERFACE,
    "SetLinkDNS");
  if ( r < 0) {
    log_error("Failed to create method call dbus message",  strerror(-r));
    goto finish;
  }

  r = sd_bus_message_append(m, "i", if_nametoindex(tunnel->ppp_iface));
  if ( r < 0) {
    log_error("Failed to modify dbus message",  strerror(-r));
    goto finish;
  }

  r = sd_bus_message_open_container(m, 'a', "(iay)");
  if ( r < 0) {
    log_error("Failed to modify dbus message",  strerror(-r));
    goto finish;
  }
  for (int i = 0; i < addresses_size; i++)
  {
          r = sd_bus_message_open_container(m, 'r', "iay");
          if ( r < 0) {
            log_error("Failed to modify dbus message",  strerror(-r));
            goto finish;
          }

          r = sd_bus_message_append(m, "i", addresses[i].sin_family);
          if ( r < 0) {
            log_error("Failed to modify dbus message",  strerror(-r));
            goto finish;
          }

          r = sd_bus_message_append_array(m, 'y', &addresses[i].ip_addr, sizeof(addresses[i].ip_addr));
          if ( r < 0) {
            log_error("Failed to modify dbus message",  strerror(-r));
            goto finish;
          }

          r = sd_bus_message_close_container(m);
          if ( r < 0) {
            log_error("Failed to modify dbus message",  strerror(-r));
            goto finish;
          }
  }
  r = sd_bus_message_close_container(m);
  if ( r < 0) {
    log_error("Failed to modify dbus message",  strerror(-r));
    goto finish;
  }

  r = sd_bus_call(bus, m, 0, &error, &reply);
  if (r < 0)
  {
    fprintf(stderr, "Failed to issue method call: %s\n", error.message);
    goto finish;
  }

finish:
  sd_bus_error_free(&error);
  sd_bus_message_unref(m);
  return r;
}

/* No action required. The settings will disapear with the link */
int systemd_del_nameservers(struct tunnel *tunnel){
  return 0;
}
