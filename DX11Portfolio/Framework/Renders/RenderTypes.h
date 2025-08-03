#pragma once

/*
enum은 전역이고, 따라서 다른 enum에서 같은 이름을 사용하면 충돌이 있지만
enum class는 명시해서 써야하므로 충돌이 없다
enum은 강제 암시적 int 캐스팅이지만 enum class는 그렇지 않다.
*/

enum class ERenderPass
{
    Opaque,
    Transparent,
    Skybox,
    Shadow,
};