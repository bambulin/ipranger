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

from random import getrandbits,randint
from ipaddress import IPv4Address, IPv6Address
import ipaddress
from tqdm import tqdm
from itertools import product
import string
import time
import random

def init_dataset(dataset):
	ids = []

	for letter in product(string.ascii_lowercase, repeat=1):
		x = ''.join(letter)
		for i in range(0,10):
			#print(x+"-"+str(i))
			ids.append(x+"-"+str(i))

	with open(dataset,"w") as d:

		for i in tqdm(range(len(ids))):
			id_ = ids[i]
			v = 4
			if i%4==0:
				mask = 32
			else:
				mask=randint(8,32)
			if v == 4:
			    bits = getrandbits(32) # generates an integer with 32 random bits
			    addr = IPv4Address(bits) # instances an IPv4Address object from those bits
			    addr_str = str(addr)+"/"+str(mask)
			    ip = ipaddress.ip_interface(unicode(addr_str)).network
			    d.write(id_+"\n"+str(ip)+"\n")
				    # else:
				    # 	print (addr_str)
				    	# get the IPv4Address object's string representation
				    	#print (addr_str)
			# elif v == 6:
			#     bits = getrandbits(128) # generates an integer with 128 random bits
			#     addr = IPv6Address(bits) # instances an IPv6Address object from those bits
			#     # .compressed contains the short version of the IPv6 address
			#     # str(addr) always returns the short address
			#     # .exploded is the opposite of this, always returning the full address with all-zero groups and so on
			#     addr_str = addr.compressed

# this adds hosts taken from networks of the initial seed into the dataset
def mutate(dataset1,dataset2):
	with open(dataset1,"r") as d, open(dataset2,"w") as d2:
		j=0
		lines = d.readlines()
		print(lines)
		dic = {}
		users = []
		ips = []
		for i in range(0,len(lines),2):
			ip = lines[i+1].rstrip("\n")
			user = lines[i].rstrip("\n")
			addr,mask = ip.split("/")
			if int(mask) != 32:
				ips.append(ipaddress.ip_network(unicode(ip)))
				users.append(user)
			d2.write(user+"\n"+ip+"\n")
			# else:
			# 	print mask
			# 	ips.append(ipaddress.IPv4Address(unicode(addr)))
			# print(user+":"+ip)
			# dic[user] = ip

		for i in tqdm(range(len(ips))):
			addresses = ips[i].num_addresses
			#maybe change this number to generate more addresses? could also be random
			if int(addresses) > 4:
				ints = 2
			else:
				ints = int(addresses)-2
			#rands=[]
			#print(ints)
			#for j in range(1,ints):
				#rands[i].append(randint(1,addresses))
				#x = ips[i].hosts()
			#x = iter_sample_fast(ips[i].hosts(),ints)
			x = random.sample(list(ips[i].hosts()),ints)
			for k in range(len(x)):
				d2.write("generated-"+str(j)+"\n"+str(x[k])+"/32"+"\n")
				j = j+1





# this will mutate in the sense that it will find subnet addresses that are not used and will add them to the test

# generate random ips and if they dont exist add them as owned by supernet

def gen_test(dataset,test):
	with open(dataset,"r") as d, open(test,"w")as t:
		lines = d.readlines()
		print(lines)
		dic = {}
		users = []
		ips = []
		hosts = []
		# get only networks and not hosts (>/32)
		for i in range(0,len(lines),2):
			ip = lines[i+1].rstrip("\n")
			user = lines[i].rstrip("\n")
			addr,mask = ip.split("/")
			if int(mask) != 32:
				ips.append(ipaddress.ip_network(unicode(ip)))
				users.append(user)
			# addresses are written as is to the test file
			else:
				hosts.append(ipaddress.ip_address(unicode(addr)))
				t.write(addr + "\n" + user + "\n")

		# print("Hosts:")
		# print(hosts)

		for i in tqdm(range(len(ips))):

			# find the subnets that already exist in the dataset
			blacklist = []
			for j in range(i+1,len(ips)):

				if ips[i].supernet_of(ips[j]) == True :
					#print(str(ips[i]) + " is a supernet of "+ str(ips[j]))
					blacklist.append(ips[j])

			# generate random addresses
			if (len(blacklist)>0):
				number_of_addresses = ips[i].num_addresses
				#maybe change this number to generate more addresses? could also be random
				if int(number_of_addresses) > 4:
					ints = 2
				else:
					ints = int(number_of_addresses)-2
				x = random.sample(list(ips[i].hosts()),ints)
				uniq = True
				tested = []

				for k in range(len(x)):
					while uniq:
						uniq = False
						for m in range(len(blacklist)):
							if x[k] in blacklist[m]:
								uniq = True
								#print ("IP is in existing subnet")

						if ipaddress.ip_address(x[k]) in hosts:
							uniq = True
							#print ("IP exists in hosts")

						if uniq is True:
							#print("retrying to sample new ip")
							x[k] = random.sample(list(ips[i].hosts()),1)[0]
							#print("Sampled IP:" +str(x[k]))

					#print("IP does not exist and will be written to file:"+str(users[i])+":"+str(x[k]))
					#t.write(users[i]+"\n"+str(x[k])+"\n")
					t.write(str(x[k])+"\n"+users[i]+"\n")








if __name__ == "__main__":
	init_dataset("dataset2.txt")
	mutate("dataset2.txt","dataset2_mutated.txt")
	gen_test("dataset2_mutated.txt","test2.txt")




