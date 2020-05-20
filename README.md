# Airplane-Game
**1. 목표**   
Win32에서 DirectSound를 이용하여 사운드를 출력한다.

**2. 요구 사항**   
다음 사항을 모두 만족하는 응용 프로그램을 작성한다.
1. 화면에 배경 이미지와 비행기 이미지를 띄운다. ‘방향키’를 누를 때마다 비행기가 움 직이고, ‘Space’를 누를 때마다 미사일이 나가야 한다.   
2. 응용 프로그램이 시작된 후, 배경 사운드가 계속 반복되어 출력된다.   
3. 비행기에서 미사일을 발사할 때마다 미사일 사운드가 출력된다.    

**3. 과정**   
1. 프로젝트에 필요한 파일들을 추가하고 설정을 변경한다.‘GP-09 resource’의 파일들을 프로젝트 폴더로 복사한다. 메인 Cpp 파일에서 ‘DXUTSound.h’를 Include 한다. 프로젝트 소스 파일 폴더에 ‘DXUTSound.cpp’를 Drag & Drop하여 추가한다. 프로젝트 속성 -> 구성 속성 -> 링커 -> 입력 -> 추가 종속성에 ‘dxguid.lib’, ‘winmm.lib’, ‘dsound.lib’를 추가한다.   
2. CSoundManager의 초기화는 WndProc 함수의 WM_CREATE case에서 수행한다. 협력 수준을 DSSCL_NORMAL로 설정한다.   
3. 배경 사운드 재생은 WndProc 함수의 WM_CREATE case에서 수행한다. Looping Playing으로 재생하여 무한 반복되도록 한다.   
4. 미사일 사운드 재생은 WndProc 함수의 WM_KEYDOWN case에서 수행한다. 미사일 사운드는 미사일이 발사될 때마다 재생되어야 한다. 매번 재생 시, Reset을 수행하여 이전 효과음을 중단시키고 새로 효과음을 재생한다.  

**4. 결과**   
![image](https://user-images.githubusercontent.com/38244836/82404959-8deec580-9a9d-11ea-982a-b77200c48029.png)
