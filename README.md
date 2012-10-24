Portal-Effect
=============

An OpenGL effects demo that attempts to replicate the portal effect from the popular video game [Portal](http://en.wikipedia.org/wiki/Portal_\(video_game\) "Link to the Portal Wiki page"). My final project in Tobias Höllerer's Computer Graphics class (Fall '08).

For those who aren't familiar with the game or the effect itself, imagine each portal is a window to the world seen by the other portal.

![Portal Image](http://www.saeedmahani.com/images/portal1.png)

## Implementation ##

My first implementation used a render-to-texture method. First the scene was rendered twice from the perspective of each portal and stored as a pair of textures. Then the textures were mapped onto opposite portals for a final rendering pass. However, there was noticeable perspective skew and some pixilation visible in the textures. Removing the projection perspective from the preliminary passes did not resolve the issue, though there may be a way of getting this method to work.

After sifting through the OpenGL documentation some more, I gave the stencil buffer a shot - a buffer used to specify portions of the screen to be rendered. This turned out to be just what I needed. In two preliminary passes, each portal is rendered to a stencil buffer from the perspective of the viewer (creating two stencils). Then, similar to before, the scene is rendered from the perspective of each portal, and these images are saved. Lastly, after a final rendering, each image is superimposed within the bounds of the opposite stencil.
