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
from tqdm import tqdm

MAX_IPV4 = ipaddress.IPv4Address._ALL_ONES  # 2 ** 32 - 1
MAX_IPV6 = ipaddress.IPv6Address._ALL_ONES  # 2 ** 128 - 1
#source: https://stackoverflow.com/questions/21014618/python-randomly-generated-ip-address-as-string


def random_str(length):
	return ''.join(random.sample(string.ascii_lowercase+string.ascii_uppercase+"!@#%^$&*()_-+=",length))

def random_num(length):
	return ''.join('%s'%random.randint(0, 9) for i in range(length))

# Arguments:
# total: the total number of tuples letters: number of letters that the user id will contain, # numbers: respective numbers for the user id
# n: number of iterations (switches of ip address per user, dataset: the output dataset file, testfile: the output test file
# version: 4 for IPv4 and 6 for IPv6 
def test3(total,letters, numbers,n,dataset,testfile,version):

	# set that holds the generated IP addresses
	ips = set()
	# list that holds the generated user IDs
	ids = set()

	# open two files for writing: input dataset and test file
	with open(dataset,"w") as d,open(testfile,"w") as t:
		print("\n[+] Generating user IDs") 
		# generate combinations of length(letters) with a vocabulary of ascii lowercase, uppercase and symbols
		for i in tqdm(range(total)):
			while True:
				id_=random_str(letters)+'-'+str(random_num(numbers))
				# store the current size of the set
				old_len = len(ids)
				# update the set 
				ids.add(id_)
				# if the new size is larger, then this is a unique id, else generate new one
				if len(ids)>old_len:
					break
		ids = list(ids)
		# perform n IP assignments/user
		print("[+] Generating User-IP assignments") 
		for rounds in range(n):
			print("Iteration #"+str(rounds))
			for i in tqdm(range(len(ids))):
				# pick a user
				id_=ids[i]
			
				# generate unique IP addresses in order to avoid collisions (same IP address to N different users)
				while True: 
					# if version equals 4, generate an IPv4 address
					if version == 4:				
						ip = str(ipaddress.IPv4Address._string_from_ip_int(random.randint(0, MAX_IPV4))) + "/32"
					# if version equals 6, generate an IPv6 address
					elif version == 6:
						ip = str(ipaddress.IPv6Address._string_from_ip_int(random.randint(0, MAX_IPV6))) + "/128"

					# store the current size of the set
					old_len = len(ips)
					# update the set 
					ips.add(ip)
					# if the new size is larger, then this is a unique IP address, else generate new one
					if len(ips)>old_len:
						break
						
				# write tuple to the dataset
				d.write(id_+"\n"+ip+"\n")

				# if this is the last round, create the test file that holds the last know address of each user
				if (rounds == n-1):
					t.write(ip+"\n"+id_+"\n")


if __name__ == "__main__":
	test3(100000,60,2,1,"dataset3v4_small.txt","test3v4_small.txt",4)
	test3(100000,60,2,1,"dataset3v6_small.txt","test3v6_small.txt",6)






