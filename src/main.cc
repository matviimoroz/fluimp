#include "fluimp.hh"

int main() {
	Fluimp fluimp;
	
	fluimp.init();
	
	while (fluimp.running) {
		fluimp.update();

		BeginDrawing();
		{
			fluimp.render();
		}
		EndDrawing();

		if (WindowShouldClose()) {
			fluimp.running = false;
		}
	}

	fluimp.cleanup();
}