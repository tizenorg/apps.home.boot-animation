#sbs-git:slp/pkgs/b/boot-animation boot-animation 0.2 d1bbca3948e4cdb6b2f9e75f176500f452fe6a33
Name: boot-animation
Version: 0.4.2
Release: 1
Summary: Boot animation
URL: http://slp-source.sec.samsung.net
Source: %{name}-%{version}.tar.gz
Source1: boot-animation-wearable.service
Source2: boot-animation-mobile.service
Source3: shutdown-animation.service
Source1001: boot-animation.manifest
Source1002: wearable/boot-animation.manifest
License: Flora Software License
Group: Application/Other
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires: cmake
BuildRequires: edje, edje-bin, embryo, embryo-bin
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(x11)
BuildRequires: pkgconfig(libexif)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(utilX)
BuildRequires: pkgconfig(vconf)
%if "%{_repository}" == "wearable"
BuildRequires: pkgconfig(mm-bootsound)
%else
BuildRequires: pkgconfig(avsystem)
BuildRequires: pkgconfig(audio-session-mgr)
%endif

Requires(post): /usr/bin/vconftool
#%if "%{_repository}" !== "wearable"
#Requires(post): /usr/bin/systemctl
#%endif

%description
Shows an animation and plays a sound when the device is booted or shutdown.

%ifarch %{arm}
%define ARCH arm
%else
%define ARCH emulator
%endif

%if "%{_repository}" == "wearable"
	%define micro_ani ON
%else
	%define micro_ani OFF
%endif

%prep
%setup -q
#%patch0 -p1


%build
%if "%{_repository}" == "wearable"
cp %{SOURCE1002} .
cd ./wearable
%else
cp %{SOURCE1001} .
cd ./mobile
%endif

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DUSE_MICRO_ANI=%{micro_ani} -DARCH=%{ARCH}

make %{?jobs:-j%jobs}

%install  
rm -rf %{buildroot}

%if "%{_repository}" == "wearable"
cd ./wearable

mkdir -p %{buildroot}/usr/share/license
cp -f LICENSE %{buildroot}/usr/share/license/%{name}
%make_install

mkdir -p %{buildroot}%{_libdir}/systemd/system/tizen-system.target.wants
install -m 0644 %SOURCE1 %{buildroot}%{_libdir}/systemd/system/boot-animation.service
ln -s ../boot-animation.service %{buildroot}%{_libdir}/systemd/system/tizen-system.target.wants/
#mkdir -p %{buildroot}%{_libdir}/systemd/system/shutdown.target.wants
install -m 0644 %SOURCE3 %{buildroot}%{_libdir}/systemd/system/shutdown-animation.service
#ln -s ../shutdown-animation.service %{buildroot}%{_libdir}/systemd/system/shutdown.target.wants/
%else
cd ./mobile
%make_install

mkdir -p %{buildroot}%{_libdir}/systemd/user/core-efl.target.wants
install -m 0644 %SOURCE2 %{buildroot}%{_libdir}/systemd/user/boot-animation.service
ln -sf ../boot-animation.service %{buildroot}%{_libdir}/systemd/user/core-efl.target.wants

mkdir -p %{buildroot}/usr/share/license
cp -f LICENSE.Flora %{buildroot}/usr/share/license/%{name}
%endif

%post
%if "%{_repository}" == "wearable"
vconftool  -i set -t int memory/boot_animation/finished 0  -u 5000 -f -s system::vconf_inhouse
%else
ln -s /etc/init.d/boot-ani /etc/rc.d/rc3.d/S41boot-ani
/usr/bin/systemctl daemon-reload
vconftool  -i set -t string memory/boot-animation/restart start:start
vconftool  -i set -t int memory/boot-animation/finished 0
%endif

%files
%manifest boot-animation.manifest
/usr/share/edje/poweroff.edj
/usr/share/edje/poweron.edj
/usr/share/license/%{name}
%{_bindir}/boot-animation
%if "%{_repository}" == "wearable"
/usr/share/keysound/poweron.wav
%{_libdir}/systemd/system/boot-animation.service
%{_libdir}/systemd/system/tizen-system.target.wants/boot-animation.service
%{_libdir}/systemd/system/shutdown-animation.service
#%{_libdir}/systemd/system/shutdown.target.wants/shutdown-animation.service
%else
/etc/init.d/boot-ani
%{_libdir}/systemd/user/boot-animation.service
%{_libdir}/systemd/user/core-efl.target.wants/boot-animation.service
%endif
