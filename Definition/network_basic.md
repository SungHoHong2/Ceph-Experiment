### Network basic controls

```
// checking network manager status "ACTIVE"
$ sudo systemctl status NetworkManager

// NetworkManager Command line interface
$ nmcli

// checking ethernet connection
$ nmcli device or nmcli d

// running GUI for network connection
$ nm-connection-editor

```

### Difference between device and connections
- Device
  - Network hardware on your computer
  - Ethernet port, Wifi Adapter
- Connection
  - Collection of settings that can be applied to your device
- **note that network managers should be restarted when it is not recognizing the devices**


```
// restart the Network manager and check it is running
  $ service network-manager restart
  $ service network-manager status

// you will find the two columns DEVICE and CONNECTION
  $ nmcli d

// connection.* // this part can be configured
  $ nmcli connection show enp0s3


// edit the connection name into myEthernet
  $ nmcli connection edit enp0s3
  $ nmcli > print connection // shows all the variables of connection
  $ nmcli > describe connection.id // describe what the connection.id does
  $ nmcli > set connection.id myEthernet // change the ethernet name into myEthernet
  $ nmcli > quit
  $ nmcli connection // you can see that the connection name has changed



// add another connection name "Othernet"
  $ nmcli connection add con-name othernet ifname enp0s3 type ethernet

```

### Static address
