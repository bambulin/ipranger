1) Create DB
```
$ ./iprangercli ./testing/config.txt -c
```

Note you can run it multiple times, overwrite, update...

2) Test
```
$ ./iprangercli ./testing/test.txt -t
```
e.g. success:

```
  Tried to match 8078:5a6c:9a02:43cb:5c51:b5cb:4151:13c5 key in vain (8078:5a6c:9a02:43cb:5c51:b5cb:4151:13c5/128)
  Used key: 8078:5a6c:9a02:43cb:5c51:b5cb:4151:13c5 Hit key: 8078:5a6c:9a02:43cb:ffff:ffff:ffff:ffff
  Found data: XXX-2 expected data: XXX-2
```

e.g. failure (found unexpected data, marked with "!"):

```
  Tried to match 8078:5a6c:9a02:43cb:5c51:b5cb:4151:13c5 key in vain (8078:5a6c:9a02:43cb:5c51:b5cb:4151:13c5/128)
  Used key: 8078:5a6c:9a02:43cb:5c51:b5cb:4151:13c5 Hit key: 8078:5a6c:9a02:43cb:ffff:ffff:ffff:ffff
! Found data: XXX-2 expected data: XXX-3333
```
