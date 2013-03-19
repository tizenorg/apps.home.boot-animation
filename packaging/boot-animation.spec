Name: boot-animation
Version: 0.4.2
Release: 1
Summary: Boot animation
Source: %{name}-%{version}.tar.gz
Source101: boot-animation.service
Source1001: packaging/boot-animation.manifest 
License:    Flora Software License
Group: Samsung/Application
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires: cmake
BuildRequires: edje, edje-bin, embryo, embryo-bin
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(x11)
BuildRequires: pkgconfig(libexif)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(utilX)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(avsystem)
BuildRequires: pkgconfig(audio-session-mgr)


Requires(post): /usr/bin/vconftool
Requires(post): /usr/bin/systemctl

%description
Shows an animation and plays a sound when the device is booted or shutdown.

%prep
%setup -q
#%patch0 -p1


cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

%build
cp %{SOURCE1001} .
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}%{_libdir}/systemd/user/core-efl.target.wants
install -m 0644 %SOURCE101 %{buildroot}%{_libdir}/systemd/user/
ln -sf ../boot-animation.service %{buildroot}%{_libdir}/systemd/user/core-efl.target.wants

mkdir -p %{buildroot}/usr/share/license
cp -f LICENSE.Flora %{buildroot}/usr/share/license/%{name}

%clean
make clean

%docs_package


%post
ln -s /etc/init.d/boot-ani /etc/rc.d/rc3.d/S41boot-ani
/usr/bin/systemctl daemon-reload
vconftool  -i set -t string memory/boot-animation/restart start:start
vconftool  -i set -t int memory/boot_animation/finished 0

%files
%manifest boot-animation.manifest
/etc/init.d/boot-ani
/usr/share/edje/poweroff.edj
/usr/share/edje/poweron.edj
/usr/bin/boot-animation
%{_libdir}/systemd/user/boot-animation.service
%{_libdir}/systemd/user/core-efl.target.wants/boot-animation.service
/usr/share/license/%{name}
