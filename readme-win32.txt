
Welcome to LundFTPD for Win32/cygwin.

To add the very first user account, use "./lundftpd.exe -a" and input the
appropriate values.

Run it initially as "./lundftpd.exe -v" to see any eventual errors. Please
change ONLY the DIRECTORY PATHS in the lundftpd.conf file _initially_. Once
you have it working then have a go at the rest, be aware it is very easy
to break things in the .conf. It is better to leave it as default unless you
know what you are doing.

If you receive a:
"999 Unable to provide directory listing! Couldn't create temporary file."
it generally means you do not have a "tmp" directory, which is where it builds
temporary files for directory listings. This should be from the root of the disk
that lundftpd.exe is run from. For example: "d:\tmp" needs to exist, as a 
directory. If you've installed cygwin, you may need "c:\cygwin\tmp" instead depending
on its installation location. Use "filemon" to work out what path it is using
should you still have this issue.

Please be aware that the default SSL/TLS certificate is only an example,
if you keep using it, it will not be secure. See the TLSreadme for information
regarding making your own certificate. This can be done on any system.

Please keep in mind about slashes (/) in all the configuration files. If the
example path has an ending '/' then yours should as well, and vice-verse. This
is particularly important for diskcull, if you want to use it. The -harmless version
is the same compile, but with the actual delete call commented out, useful to check
your configuration file is correct. Note however, as the diskspace isn't going down
(we aren't calling delete after all) you will find that it will just keep (trying to)
delete things making it look like it would wipe everything. This should not happen
when you run the live version as it is actually freeing up space. Note that it calls
'mv' and 'rm' and would need to be run within a cygwin shell environment.

Soft-link (or short cuts) will work, however if you make them with Windows they'll have
the ever so attractive ".lnk" extention on them. If you bother to get cygwin, you can
make soft-links within that (using 'ln -s') which will work with systems.


Good luck.

lundman@lundman.net
