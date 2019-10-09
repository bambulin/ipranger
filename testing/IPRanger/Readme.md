## IPRanger Tests

### Usage

Configure the db:
```
./iprangercli ../../../testing/IPRanger/test0/dataset0.txt -c
```

Test it:
```
./iprangercli ../../../testing/IPRanger/test0/test0.txt -t | tee log.log
```
Any failures, mismatches, are marked with ```!```, e.g.

```
cat log.log  | grep  '!'
```
Example of a failure:

```
! Address: 16ee:cd3f:445e:906f:ffff:ffff:ffff:ffff Found data: XXX-16 expected data: XXX-1
```

### Test0 - contains both IPv4 and IPv6
#### Scenario:
This test creates a small volume of users (6760) that are frequently assigned random address.
In more detail, it tests only the assignment of address, so the last address of each user should be the correct.
The created addresses are unique and have a mask of /32 for IPv4 or /128 for IPv6 so **we are not covering the case of subnets**.

#### Dataset File:
We are creating 67600 tuples of <user:address> so each user in average changes 10 times address.

Lines =  135200 (2 lines/tuple)

#### Test File:
The last address of each user.

Lines = the users that are assigned addresses are random so it varies.

### Test1 - contains both IPv4 and IPv6
#### Scenario:
This test aims to create a small volume of IP addresses (6760) that frequently change users - the case where an IP address is freed and allocated to a different user.
Test1 is about the rotation of users, so the last owner of each address should be the correct.
The created addresses are unique and have a mask of /32 or /128 so we are not covering the case of subnets.

#### Dataset File:

Lines: 6760 tuples * 2 lines/tuple  + 10,000 exchanges * 4 lines/exchange = 53520

There is the initial arrangement of the tuples and 10,000 simulated (ex)changes of IPs.

#### Test File:

Lines: 6760 tuples * 2 lines/tuple

The last address of each user.

### Test2 - contains only IPv4

#### Scenario

This test aims to check for IPs that coexist in network segments.
For example an IP that is provided by a Telco and exists also in an ISP.

Assume that all networks are smaller that /8 and that 1 in 4 addresses will be a host (/32)

To further elaborate, a dataset is created in the beginning that contains IP networks and hosts.
Based on this dataset, a mutation takes place and new hosts are added to the dataset. These hosts come from the networks that are already present. For example if there is a network 192.168.1.0/24, the IP address 192.167.1.15 might be added in this step.

 Last but not least, the test file is created. For this to be implemented, the supernets are located and for each one, its subnets and the hosts that are already present in the dataset are ruled out (red address space) . The final result is a set of IP addresses that belong to the supernet and are not yet assigned to a subnet (white address space).

#### Dataset Files:

Initial dataset

Mutated dataset

#### Test File:

Contains the mutated hosts as is.

### Test3 - Performance - contains only IPv4

### Scenario

Dataset includes large names(less than 64 chars) in order to measure the scalability. Similar to Test0. Will be created an equivalent to Test2 (with overlapping networks)

### Changelog

20191004 - Small Fix in Test2 - print network and user in correct order @testfile