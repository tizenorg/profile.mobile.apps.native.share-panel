Name:       org.tizen.share-panel
#VCS_FROM:   profile/mobile/apps/native/share-panel#598e9ea6cb6b48a8030f3478da46342c890339e6
#RS_Ver:    20160314_7
Summary:    App in rpm
Version:    1.0.0
Release:    1
Group:      N/A
License:    N/A
Source0:    %{name}-%{version}.tar.gz

%ifarch i386 i486 i586 i686 x86_64
%define target i386
%else
%ifarch arm armv7l aarch64
%define target arm
%else
%define target noarch
%endif
%endif

%description
TPK in RPM sample

%prep
%setup -q

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/opt/usr
install %{name}-%{version}-%{target}.tpk %{buildroot}/opt/usr/

%post
/usr/bin/tpk-backend -i /opt/usr/%{name}-%{version}-%{target}.tpk --preload
rm -rf /opt/usr/%{name}-%{version}-%{target}.tpk

%files
%defattr(-,root,root,-)
/opt/usr/*
