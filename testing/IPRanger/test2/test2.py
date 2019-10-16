# Copyright (C) 2019 Contributors to ipranger project.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# author: Andronikos Kyriakou

import string
import random
import ipaddress
import netaddr
from datetime import datetime
from tqdm import tqdm
from itertools import product

MAX_IPV4 = ipaddress.IPv4Address._ALL_ONES  # 2 ** 32 - 1
MAX_IPV6 = ipaddress.IPv6Address._ALL_ONES  # 2 ** 128 - 1

#ip generation code from: https://codereview.stackexchange.com/questions/200337/random-ip-address-generator


# Initialize the dataset
# Arguments:
# letters: number of letters that the user id will contain, numbers: respective numbers for the user id
# dataset: the output dataset file
# version: 4 for IPv4 or 6 for IPv6 
def init_dataset(letters,numbers,dataset,version):
	# list tha holds the generated user IDs
	ids = []

	# create a set to store unique ips
	ips = set()


	# counter for the generated ip addresses
	j = 0
	print("[+] Generating user IDs") 
	# generate all the possible combinations of #letters with a vocabulary of ascii lowercase charactes
	for letter in product(string.ascii_lowercase, repeat=letters):
		x = ''.join(letter)
		for i in range(pow(10,numbers)):
			ids.append(x+"-"+str(i))

	# open the dataset for writing
	with open(dataset,"w") as d:
		print("[+] Generating initial IPs") 
		
		for i in tqdm(range(len(ids))):
			
			random.seed(datetime.now())
			old_length = len(ips)
			# guarantee uniqueness in the gnerated addresses
			while (old_length == len(ips)):

				# generate with 25% prob a host and with 75% prob a network
				if version == 4:
					if i%4 == 0:
						mask = 32
					else:
						mask = random.randint(1,31)
					# generate 32 random bits
					addr = str(ipaddress.IPv4Address._string_from_ip_int(random.randint(0, MAX_IPV4)))

				elif version == 6:
					if i%4 == 0:
						mask = 128
					else:
						mask = random.randint(100,127)
					addr = str(ipaddress.IPv6Address._string_from_ip_int(random.randint(0, MAX_IPV6)))
				
				# create a sting that holds the generated address and the random mask
				addr_str = str(addr)+"/"+str(mask)
				# get the network equivalent address, if this is a host address, it remains the same
				ip = ipaddress.ip_interface(unicode(addr_str)).network
				ips.add(ip)

			# write output to the dataset
			d.write(ids[i]+"\n"+str(ip)+"\n")


			if (version == 4):
				if(mask != 32):
					# convert ip interface to network
					ip_net = ipaddress.IPv4Network(unicode(ip))
					# get the number of hosts in the network
					number_of_addresses = ip_net.num_addresses
					# if the number of hosts is greater that 10, then generate 10 addresses
					if mask < 28:
						to_be_generated = 10
					elif mask < 30:
						to_be_generated = 4
					# in the case of /31 generate the 2nd available host
					else:
						to_be_generated = 1
					# use a set in order to guarantee uniqueness 
					generated = set()
					while(len(generated) < to_be_generated):
						bits = random.getrandbits(ip_net.max_prefixlen - ip_net.prefixlen)
						generated.add(ipaddress.IPv4Address(ip_net.network_address + bits))
					# convert to a list and write to file
					x = list(generated)
					for k in range(len(generated)):
						d.write("generated-"+str(j)+"\n"+str(x[k])+"/32"+"\n")
						j = j + 1

			elif (version == 6):
				if(mask != 128):
					# convert ip interface to network
					ip_net = ipaddress.IPv6Network(unicode(ip))
					# get the number of hosts in the network
					number_of_addresses = ip_net.num_addresses
					# if the number of hosts is greater that 10, then generate 10 addresses
					if mask < 125:
						to_be_generated = 10
					elif mask < 127:
						to_be_generated = 2
					else:
						to_be_generated = 1
					
					generated = set()
					while(len(generated) < to_be_generated):
						bits = random.getrandbits(ip_net.max_prefixlen - ip_net.prefixlen)
						generated.add(ipaddress.IPv6Address(ip_net.network_address + bits))

					x = list(generated)
					for k in range(len(generated)):
						d.write("generated-"+str(j)+"\n"+str(x[k])+"/128"+"\n")
						j = j + 1




# this will create the test file by finding addresses in subbnets that are not alocated and will add them to the test 

# generate random ips and if they dont exist add them as owned by supernet

def gen_test(dataset,test,version):
	with open(dataset,"r") as d, open(test,"w")as t:
		lines = d.readlines()
		users = []
		ips = []
		hosts = set()
		# get only networks and not hosts (!= /32 or != /128)
		for i in range(0,len(lines),2):
			user = lines[i].rstrip("\n")
			ip = lines[i+1].rstrip("\n")
			addr,mask = ip.split("/")
			if (version == 4 and int(mask)!=32) :
				ips.append(ipaddress.IPv4Network(ipaddress.ip_interface(unicode(ip)).network))
				users.append(user)	
			elif (version == 6 and int(mask)!=128):
				ips.append(ipaddress.IPv6Network(ipaddress.ip_interface(unicode(ip)).network))
				users.append(user)	

			else:
				# add host address to set for later comparison
				hosts.add(ipaddress.ip_address(unicode(addr)))
				# host addresses are written as is to the test file
				t.write(ip + "\n" + user + "\n")

		# for all the networks that exist in the dataset
		print("[+] Creating Test Cases")
		for i in tqdm(range(len(ips))):
			# find the subnets that already exist in the dataset and create a "blacklist"
			blacklist = []

			for j in range(i+1,len(ips)):
				# if the next items in the list are a subnet of the current network, add them to blacklist in order not to sample IP addresses from this network area
				if ips[j].subnet_of(ips[i]) == True:
					blacklist.append(ips[j])

			# generate random addresses
			if (len(blacklist)>0):
				number_of_addresses = ips[i].num_addresses

				# add at most 2 addresses in the test - many are added during the previous generation, this number can be increased for more test cases
				if int(number_of_addresses) > 16:
					to_be_generated = 2
				# use a set in order to guarantee uniqueness 
				generated = set()
				while(len(generated) < to_be_generated):
					bits = random.getrandbits(ips[i].max_prefixlen - ips[i].prefixlen)
					if version == 4:
						generated.add(ipaddress.IPv4Address(ips[i].network_address + bits))
					elif version == 6:
						generated.add(ipaddress.IPv6Address(ips[i].network_address + bits))
				# convert to a list
				x = list(generated)				
			
				for k in range(len(x)):
					# generate unique host address in the subnet, try at most 3 times to avoid endless loops
					tries = 0 
					uniq = False
					while not uniq and tries < 3:
						tries = tries + 1
						uniq = True
						# test if address is in a subnet that exists in a blacklist
						for m in range(len(blacklist)):
							if x[k] in blacklist[m]:
								uniq = False
								#print ("IP address exists in a subnet")

						# test if the generated ip address is in the existing hosts
						if ipaddress.ip_address(x[k]) in hosts:
							uniq = False
							#print ("IP address exists in hosts")

						# retry to sample IP address if it is not unique
						if uniq is False:
							#print("retrying to sample ip")
							bits = random.getrandbits(ips[i].max_prefixlen - ips[i].prefixlen)
							if version == 4:
								x[k]= ipaddress.IPv4Address(ips[i].network_address + bits)
							elif version == 6:
								x[k]= ipaddress.IPv6Address(ips[i].network_address + bits)
							#print("Sampled IP:" +str(x[k]))

					#print("IP does not exist and will be written to file:"+str(users[i])+":"+str(x[k]))
					# write IP in test file
					if version == 4:
						t.write(str(x[k])+"/32"+"\n"+users[i]+"\n")
					else:
						t.write(str(x[k])+"/128"+"\n"+users[i]+"\n")
					# append the generated IPs in the hosts in order not to add them again in later iteration
					hosts = hosts.union(set(x))



if __name__ == "__main__":

	init_dataset(1,1,"dataset2v4_small.txt",4)
	gen_test("dataset2v4_small.txt","test2v4_small.txt",4)
	init_dataset(1,1,"dataset2v6_small.txt",6)
	gen_test("dataset2v6_small.txt","test2v6_small.txt",6)

	init_dataset(2,2,"dataset2v4_large.txt",4)
	gen_test("dataset2v4_large.txt","test2v4_large.txt",4)
	init_dataset(2,2,"dataset2v6_large.txt",6)
	gen_test("dataset2v6_large.txt","test2v6_large.txt",6)




