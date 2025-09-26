#pragma once

void EnableConsole()
{
	// WinMain에서 콘솔을 열기 위한 조합
	// 사실 원래 쓰는 콘솔 앱 main()에서는 자동으로 해줬던 것이다.
	// 1. 콘솔창을 열고
	// 2. 입력/출력/에러를 CRT에 바인딩. 
	// 콘솔앱과 입출력 스트림을 연결

	AllocConsole();
	freopen("CONOUT$", "w", stdout);  // std::cout
	freopen("CONOUT$", "w", stderr);  // std::cerr
	freopen("CONIN$", "r", stdin);    // std::cin
}

void DisableConsole()
{
	// stdin은 비우지 않아도 되는 이유는 입력버퍼는 출력 버퍼가 없으니까
	fflush(stdout); fflush(stderr); 
	fclose(stdout); fclose(stderr); fclose(stdin); 
	FreeConsole();
}