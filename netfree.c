#include <pcap.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <curl/curl.h>
#include <stdbool.h>

#include "netfree.h"
#include "mac.h"
#include "scanner.h"
#include "MacQueue.h"


/**
 * Resets the device's MAC address to its original MAC address and exits the system with
 * the status code provided.
 *
 * @param exitCode (int) - the status with which the program should exit
 */
void exitSystem(int exitCode) {
  fprintf(stderr, "\n\nExiting NetFree\n");

  char originalMacAddress[NETFREE_MAC_SIZE];
  getOriginalMacAddress(originalMacAddress);

  destroyScanner();

  fprintf(stderr, "Resetting to original MAC address.\n");
  setDeviceMacAddress(originalMacAddress);

  destroyMac();

  exit(exitCode);
}

/**
 * Determines if the device is connected to the Internet.  This method is flawed and should
 * be used in combination with the user checking for him- or herself if the device is indeed
 * connected to the Internet.
 *
 * @return (bool) Iff the device is connected to the Internet, true is returned.  Otherwise,
 *  false is returned.
 */
bool isConnectedToInternet() {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "www.google.com");    // Trusty, simple ol' google.com.

    res = curl_easy_perform(curl);
    if(res == CURLE_OK) {
      long responseCode;

      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
      if(responseCode == 200) {
        curl_easy_cleanup(curl);

        return true;
      }
    }
  }

  curl_easy_cleanup(curl);

  return false;
}

int main(int argc, char **argv) {
  char *iface;
  struct sigaction interruptHandler;
  int status;

  if(argc == 1) {
    fprintf(stdout, "Using default iface: " DEFAULT_IFACE "\n");

    iface = (char *) malloc((strlen(DEFAULT_IFACE) + 1) * sizeof(char));
    strcpy(iface, DEFAULT_IFACE);
  } else {
    fprintf(stdout, "Using iface: %s\n", argv[1]);

    iface = argv[1];
  }

  initMac(iface);

  char originalMacAddress[NETFREE_MAC_SIZE];
  getCurrentMacAddress(originalMacAddress);
  fprintf(stderr, "Original MAC Address: " NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(originalMacAddress));

  // Intercept interrupts so the original MAC can be reset.
  memset(&interruptHandler, 0, sizeof(interruptHandler));
  interruptHandler.sa_handler = exitSystem;
  sigemptyset(&(interruptHandler.sa_mask));
  interruptHandler.sa_flags = SA_RESTART;

  sigaction(SIGINT, &interruptHandler, NULL);
  sigaction(SIGTERM, &interruptHandler, NULL);
  sigaction(SIGQUIT, &interruptHandler, NULL);
  sigaction(SIGHUP, &interruptHandler, NULL);
  sigaction(SIGTSTP, &interruptHandler, NULL);

  status = initScanner(iface);
  if(status != 0) {
    exitSystem(status);
  }

  scan();

  char macAddress[NETFREE_MAC_SIZE];
  while(true) {
    dequeueMac(macAddress);
    status = setDeviceMacAddress(macAddress);

    if(status) {
      continue;
    }

    if(isConnectedToInternet()) {
      char command;

      printf("I think I found one.  Check it out and let me know if it works or if you want to exit and reset the system back to normal.\n");
      printf("Next/Quit (N/Q): ");
      scanf("%c", &command);

      if(command == 'Q' || command == 'q') {
        break;
      }
    }
  }

  destroyScanner();

  exitSystem(0);
}