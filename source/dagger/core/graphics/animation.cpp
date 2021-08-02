#include "animation.h"

#include "core/graphics/animations.h"

void dagger::AnimatorPlayOnce(Animator& animator_, String animationName_)
{
	if (animationName_ == animator_.currentAnimation)
		return;

	animator_.shouldLoop = false;
	animator_.currentAnimation = animationName_;
	animator_.currentFrame = -1;
	animator_.currentFrameTime = 0;
	animator_.isPlaying = true;
}

void dagger::AnimatorPlay(Animator& animator_, String animationName_)
{
	if (animationName_ == animator_.currentAnimation)
		return;

	animator_.shouldLoop = true;
	animator_.currentAnimation = animationName_;
	animator_.currentFrame = -1;
	animator_.currentFrameTime = 0;
	animator_.isPlaying = true;
}

void dagger::AnimatorStop(Animator& animator_)
{
	animator_.isPlaying = false;
}
