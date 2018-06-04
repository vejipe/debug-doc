Debug library tutorial
======================

The purpose of this document is to introduce the idea of building a special shared library alongside a software project. The purpose of this library is to facilitate debugging and test fixes.

This document refers to a demo project.

.. toctree::
   :maxdepth: 2

Demo project
------------

Build the demo project::

	mkdir build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Debug ..
	make

Note: build with debug symbols.

Build artifacts
^^^^^^^^^^^^^^^

Build artifacts:

``main/comp/data/libdata.a``
	Static library for *data* component.

``main/comp/proc/libproc.a``
	Static library for *proc* component.

``main/main``
	Main binary which includes the two static libs.

``debug/libdebug.so``
	Shared library which includes the two statics libs.

Building the shared library
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Note that static libraries are embedded in both the *main binary* and the *shared library*.

A shared library needs to be made of *position independent code* (i.e., ``PIC``) in order to be loaded at runtime. Therefore the static libraries need to be *position independent* too, so as to be embdedded in the shared library.

If it is not the case, the following link error will be raised::

	[ 88%] Linking CXX shared library libdebug.so
	/usr/bin/ld: ../main/comp/proc/libproc.a(Proc.cpp.o): relocation R_X86_64_32 against `.rodata' can not be used when making a shared object; recompile with -fPIC
	debug/CMakeFiles/debug.dir/build.make:96: recipe for target 'debug/libdebug.so.0.1' failed
	CMakeFiles/Makefile2:257: recipe for target 'debug/CMakeFiles/debug.dir/all' failed
	Makefile:83: recipe for target 'all' failed
	../main/comp/proc/libproc.a: error adding symbols: Bad value
	collect2: error: ld returned 1 exit status
	make[2]: *** [debug/libdebug.so.0.1] Error 1
	make[1]: *** [debug/CMakeFiles/debug.dir/all] Error 2
	make: *** [all] Error 2

The "``cmake`` idiomatic way"[#]_ to pass the link parameter ``-fPIC`` as it is suggested by the error message::

	set_property(TARGET data PROPERTY POSITION_INDEPENDENT_CODE ON)
	set_property(TARGET proc PROPERTY POSITION_INDEPENDENT_CODE ON)

Normal execution
----------------

During normal execution, the ``main`` binary will enter an infinite loop, and do the same every second.

.. literalinclude:: ../main/main.cpp
   :language: cpp
   :emphasize-lines: 20-27
   :linenos:

Terminal output::

	$ main/main 
	{ 1 2 3 4 }
	{ 1 2 3 4 }
	{ 1 2 3 4 }
	{ 1 2 3 4 }
	...

The goal of the shared library is to allow us to add a modified ``Data`` instance in the ``Proc`` list at runtime with the debugger.

Debug sessions
--------------

With gdb
^^^^^^^^

::

	$ gdb -q
	(gdb) set environment LD_PRELOAD debug/libdebug.so
	(gdb) file main/main 
	Reading symbols from main/main...done.
	(gdb) b main.cpp:23
	Breakpoint 1 at 0x40add8: file /home/vjp/debug/main/main.cpp, line 23.
	(gdb) r
	Starting program: /home/vjp/debug-build/main/main 

	Breakpoint 1, main () at /home/vjp/debug/main/main.cpp:23
	23	        p.run();
	(gdb) c
	Continuing.
	{ 1 2 3 4 } 
	
	Breakpoint 1, main () at /home/vjp/debug/main/main.cpp:23
	23	        p.run();
	(gdb) call p.clear()
	(gdb) call addToProc(&p, d._M_ptr)
	(gdb) c
	Continuing.
	{ 1 2 3 } 

	Breakpoint 1, main () at /home/vjp/debug/main/main.cpp:23
	23	        p.run();
	(gdb) c
	Continuing.
	{ 1 2 3 4 } 

	Breakpoint 1, main () at /home/vjp/debug/main/main.cpp:23
	23	        p.run();
	(gdb) 


Preload the shared library::

	(gdb) set environment LD_PRELOAD debug/libdebug.so

Load the main binary::

	(gdb) file main/main 
	Reading symbols from main/main...done.

Set a breakpoint in the main loop::

	(gdb) b main.cpp:23
	Breakpoint 1 at 0x40add8: file /home/vjp/debug/main/main.cpp, line 23.

Run and meet the breakpoint::

	(gdb) r
	Starting program: /home/vjp/debug-build/main/main 

	Breakpoint 1, main () at /home/vjp/debug/main/main.cpp:23
	23	        p.run();

Continue, meet the breakpoint again and check the normal output::

	(gdb) c
	Continuing.
	{ 1 2 3 4 } 
	
	Breakpoint 1, main () at /home/vjp/debug/main/main.cpp:23
	23	        p.run();

Clear the current ``Proc`` list::

	(gdb) call p.clear()

Add a new modified ``Data`` to the current list::

	(gdb) call addToProc(&p, d._M_ptr)

Continue and see the modified ``Data`` being processed::

	(gdb) c
	Continuing.
	{ 1 2 3 } 

	Breakpoint 1, main () at /home/vjp/debug/main/main.cpp:23
	23	        p.run();

Continue and check that initial data is still processed the same::

	(gdb) c
	Continuing.
	{ 1 2 3 4 } 

	Breakpoint 1, main () at /home/vjp/debug/main/main.cpp:23
	23	        p.run();

With gdb and gdbserver
^^^^^^^^^^^^^^^^^^^^^^

This trick can also be done on a running process.

Run the ``main`` binary::

	LD_PRELOAD=debug/libdebug.so ./main/main

In a separate terminal, attach to the running process::

	sudo gdbserver :3333 --attach `pidof main`
	Attached; pid = 15795
	Listening on port 3333

In a third terminal, connect ``gdb`` to the ``gdbserver``::

	$ gdb -q
	(gdb) set solib-search-path /home/vjp/debug-build/debug/
	(gdb) target remote :3333
	Remote debugging using :3333
	Reading /home/vjp/debug-build/main/main from remote target...
	warning: File transfers from remote targets can be slow. Use "set sysroot" to access files locally instead.
	Reading /home/vjp/debug-build/main/main from remote target...
	Reading symbols from target:/home/vjp/debug-build/main/main...done.
	Reading /usr/lib/x86_64-linux-gnu/libstdc++.so.6 from remote target...
	Reading /lib/x86_64-linux-gnu/libgcc_s.so.1 from remote target...
	Reading /lib/x86_64-linux-gnu/libc.so.6 from remote target...
	Reading /lib/x86_64-linux-gnu/libm.so.6 from remote target...
	Reading /lib64/ld-linux-x86-64.so.2 from remote target...
	Reading symbols from /home/vjp/debug-build/debug/libdebug.so.0.1...done.
	Reading symbols from target:/usr/lib/x86_64-linux-gnu/libstdc++.so.6...Reading /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.21 from remote target...
	Reading /usr/lib/x86_64-linux-gnu/.debug/libstdc++.so.6.0.21 from remote target...
	(no debugging symbols found)...done.
	Reading symbols from target:/lib/x86_64-linux-gnu/libgcc_s.so.1...Reading /lib/x86_64-linux-gnu/.debug/libgcc_s.so.1 from remote target...
	(no debugging symbols found)...done.
	Reading symbols from target:/lib/x86_64-linux-gnu/libc.so.6...Reading /lib/x86_64-linux-gnu/libc-2.23.so from remote target...
	Reading /lib/x86_64-linux-gnu/.debug/libc-2.23.so from remote target...
	Reading symbols from /usr/lib/debug/lib/x86_64-linux-gnu//libc-2.23.so...done.
	done.
	Reading symbols from target:/lib/x86_64-linux-gnu/libm.so.6...Reading /lib/x86_64-linux-gnu/libm-2.23.so from remote target...
	Reading /lib/x86_64-linux-gnu/.debug/libm-2.23.so from remote target...
	Reading symbols from /usr/lib/debug/lib/x86_64-linux-gnu//libm-2.23.so...done.
	done.
	Reading symbols from target:/lib64/ld-linux-x86-64.so.2...Reading /lib64/ld-2.23.so from remote target...
	Reading /lib64/.debug/ld-2.23.so from remote target...
	(no debugging symbols found)...done.
	Reading /lib64/ld-linux-x86-64.so.2 from remote target...
	0x00007f5aa65f32f0 in __nanosleep_nocancel () at ../sysdeps/unix/syscall-template.S:84
	84	../sysdeps/unix/syscall-template.S: No such file or directory.
	(gdb) f 2
	#2  0x000000000040ae16 in main () at /home/vjp/debug/main/main.cpp:26
	26	        std::this_thread::sleep_for(std::chrono::seconds(1));
	(gdb) n
	std::this_thread::sleep_for<long, std::ratio<1l, 1l> > (__rtime=...)
		at /usr/include/c++/5/thread:296
	296	      }
	(gdb) n
	main () at /home/vjp/debug/main/main.cpp:20
	20	    while(true)
	(gdb) n
	22	        p.add(*d);
	(gdb) n
	23	        p.run();
	(gdb) call p.clear()
	(gdb) call addToProc(&p, d._M_ptr)
	(gdb) c
	Continuing.

Output of the ``main`` binary::

	...
	{ 1 2 3 4 } 
	{ 1 2 3 4 } 
	{ 1 2 3 } 
	{ 1 2 3 4 } 
	{ 1 2 3 4 } 
	...

Load libdebug.so dynamically at runtime
"""""""""""""""""""""""""""""""""""""""

This only works if the ``main`` binary was built with ``-ldl`` for *dynamic loader* support.

Check for the following line in the ``gdb`` library loading::

	Reading /lib64/ld-linux-x86-64.so.2 from remote target...

``gdb`` session::

	$ gdb -q
	(gdb) target remote :3333
	Remote debugging using :3333
	Reading /home/vjp/debug-build/main/main from remote target...
	warning: File transfers from remote targets can be slow. Use "set sysroot" to access files locally instead.
	Reading /home/vjp/debug-build/main/main from remote target...
	Reading symbols from target:/home/vjp/debug-build/main/main...done.
	Reading /lib/x86_64-linux-gnu/libdl.so.2 from remote target...
	Reading /usr/lib/x86_64-linux-gnu/libstdc++.so.6 from remote target...
	Reading /lib/x86_64-linux-gnu/libgcc_s.so.1 from remote target...
	Reading /lib/x86_64-linux-gnu/libc.so.6 from remote target...
	Reading /lib64/ld-linux-x86-64.so.2 from remote target...
	Reading /lib/x86_64-linux-gnu/libm.so.6 from remote target...
	Reading symbols from target:/lib/x86_64-linux-gnu/libdl.so.2...Reading /lib/x86_64-linux-gnu/libdl-2.23.so from remote target...
	Reading /lib/x86_64-linux-gnu/.debug/libdl-2.23.so from remote target...
	Reading symbols from /usr/lib/debug/lib/x86_64-linux-gnu//libdl-2.23.so...done.
	done.
	Reading symbols from target:/usr/lib/x86_64-linux-gnu/libstdc++.so.6...Reading /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.21 from remote target...
	Reading /usr/lib/x86_64-linux-gnu/.debug/libstdc++.so.6.0.21 from remote target...
	(no debugging symbols found)...done.
	Reading symbols from target:/lib/x86_64-linux-gnu/libgcc_s.so.1...Reading /lib/x86_64-linux-gnu/.debug/libgcc_s.so.1 from remote target...
	(no debugging symbols found)...done.
	Reading symbols from target:/lib/x86_64-linux-gnu/libc.so.6...Reading /lib/x86_64-linux-gnu/libc-2.23.so from remote target...
	Reading /lib/x86_64-linux-gnu/.debug/libc-2.23.so from remote target...
	Reading symbols from /usr/lib/debug/lib/x86_64-linux-gnu//libc-2.23.so...done.
	done.
	Reading symbols from target:/lib64/ld-linux-x86-64.so.2...Reading /lib64/ld-2.23.so from remote target...
	Reading /lib64/.debug/ld-2.23.so from remote target...
	(no debugging symbols found)...done.
	Reading symbols from target:/lib/x86_64-linux-gnu/libm.so.6...Reading /lib/x86_64-linux-gnu/libm-2.23.so from remote target...
	Reading /lib/x86_64-linux-gnu/.debug/libm-2.23.so from remote target...
	Reading symbols from /usr/lib/debug/lib/x86_64-linux-gnu//libm-2.23.so...done.
	done.
	Reading /lib64/ld-linux-x86-64.so.2 from remote target...
	0x00007f39e21bb2f0 in __nanosleep_nocancel () at ../sysdeps/unix/syscall-template.S:84
	84	../sysdeps/unix/syscall-template.S: No such file or directory.
	(gdb) bt
	#0  0x00007f39e21bb2f0 in __nanosleep_nocancel () at ../sysdeps/unix/syscall-template.S:84
	#1  0x000000000040b35e in std::this_thread::sleep_for<long, std::ratio<1l, 1l> > (__rtime=...)
		at /usr/include/c++/5/thread:292
	#2  0x000000000040ae9b in main (argc=1) at /home/vjp/debug/main/main.cpp:26
	(gdb) f 2
	#2  0x000000000040ae9b in main (argc=1) at /home/vjp/debug/main/main.cpp:26
	26	        std::this_thread::sleep_for(std::chrono::seconds(1));
	(gdb) n
	std::this_thread::sleep_for<long, std::ratio<1l, 1l> > (__rtime=...)
		at /usr/include/c++/5/thread:296
	296	      }
	(gdb) n
	main (argc=1) at /home/vjp/debug/main/main.cpp:20
	20	    while(true)
	(gdb) n
	22	        p.add(*d);
	(gdb) n
	23	        p.run();
	(gdb) set solib-search-path /home/vjp/debug-build/debug/
	Reading /lib/x86_64-linux-gnu/libdl.so.2 from remote target...
	Reading /usr/lib/x86_64-linux-gnu/libstdc++.so.6 from remote target...
	Reading /lib/x86_64-linux-gnu/libgcc_s.so.1 from remote target...
	Reading /lib/x86_64-linux-gnu/libc.so.6 from remote target...
	Reading /lib64/ld-linux-x86-64.so.2 from remote target...
	Reading /lib/x86_64-linux-gnu/libm.so.6 from remote target...
	Reading /lib64/ld-linux-x86-64.so.2 from remote target...
	(gdb) call dlopen("debug/libdebug.so", 2)
	Reading symbols from /home/vjp/debug-build/debug/libdebug.so.0.1...done.
	$1 = 38973680
	(gdb) call p.clear()
	(gdb) call addToProc(&p, d._M_ptr)
	(gdb) c

Note ``dlopen`` [#]_ loads the shared library at runtime.

With lldb
^^^^^^^^^

``gdb`` to ``lldb`` command map is at [#]_

::

	$ lldb
	(lldb) env LD_PRELOAD=/home/vjp/debug-build/debug/libdebug.so
	(lldb) target create /home/vjp/debug-build/main/main
	Current executable set to '/home/vjp/debug-build/main/main' (x86_64).
	(lldb) b main.cpp:23
	Breakpoint 1: where = main`main + 106 at main.cpp:23, address = 0x000000000040ae5d
	(lldb) r
	Process 23632 launched: '/home/vjp/debug-build/main/main' (x86_64)
	Process 23632 stopped
	* thread #1, name = 'main', stop reason = breakpoint 1.1
		frame #0: 0x000000000040ae5d main`main(argc=1, (null)=0x00007fffffffdd48) at main.cpp:23
	   20  	    while(true)
	   21  	    {
	   22  	        p.add(*d);
	-> 23  	        p.run();
	   24  	        p.clear();
	   25  	
	   26  	        std::this_thread::sleep_for(std::chrono::seconds(1));
	(lldb) p p.clear()
	(lldb) p d._M_ptr
	(data::Data *) $0 = 0x0000000000624c30
	(lldb) p &p
	(proc::Proc *) $1 = 0x00007fffffffdc30
	(lldb) p addToProc($1,$0)
	error: no matching function for call to 'addToProc'
	candidate function not viable: no known conversion from 'proc::Proc *' to 'proc::Proc *' for 1st argument
	(lldb) p addToProc(&p,d._M_ptr)
	error: no matching function for call to 'addToProc'
	candidate function not viable: no known conversion from 'proc::Proc *' to 'proc::Proc *' for 1st argument

Strange error message: ``no known conversion from 'proc::Proc *' to 'proc::Proc *'``.

With lldb and gdbserver
^^^^^^^^^^^^^^^^^^^^^^^

``TODO``

References
==========

.. [#] https://stackoverflow.com/questions/38296756/what-is-the-idiomatic-way-in-cmake-to-add-the-fpic-compiler-option
.. [#] http://man7.org/linux/man-pages/man3/dlopen.3.html
.. [#] https://lldb.llvm.org/lldb-gdb.html
