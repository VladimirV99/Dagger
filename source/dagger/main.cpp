#include "core/engine.h"
#include "gameplay/atonement/atonement_main.h"
#include "gameplay/editor/editor_main.h"

int main(int argc_, char** argv_)
{
	dagger::Engine engine;
	return engine.Run<atonement::AtonementGame>();
	//return engine.Run<editor::EditorTestGame>();
}
