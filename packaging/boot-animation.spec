Name: boot-animation
Version: 0.3.0
Release: 1
Summary: Boot animation
Source: %{name}-%{version}.tar.gz
License:    Samsung Proprietary License
Group:    TO_BE/FILLED_IN
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

%description
Shows an animation and plays a sound when the device is booted or shutdown.

%prep
%setup -q
#%patch0 -p1


cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

%build
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%clean
make clean

%docs_package


%post
ln -s /etc/init.d/boot-ani /etc/rc.d/rc3.d/S41boot-ani

vconftool  -i set -t string memory/boot-animation/restart start:start


%files
/etc/init.d/boot-ani
/usr/share/edje/poweroff.edj
/usr/share/edje/poweron.edj
/usr/bin/boot-animation
