

## Table of contents
[I. Glossary](#I.%20glossary)
[II. Parallelization](#II%20Parallelization)
[III. Resource management](#III.%20Resource%20management)
[IV. Data oriented design](#IV.%20Data%20oriented%20design)
[V. Renderer-Master differentiation](#V.%20Renderer-Master%20differentiation)
[VI. Components](#VI.%20Components)

#### Note:
The engine is still very much in progress and VERY early in development as of writing this so don't read this like a rule book, many things might change and I don't have the power of hindsight.

#### Terms
- master/manager (used interchangably) - the main thread and engine process
- renderer - the "secondary" process where all the rendering happens

## I. Glossary
### 1. Segmentation
We've made a very interesting *(if not extreme and a bit stupid)* choice to separate our engine into multiple parts comprised of shared object files.
We did it for a couple of reasons:
- Easier updates
- *Insanely* good modularity
- it's cool
As of now the parts of our engines that are "modular" are: the renderer, all of the components, the "app" itself.

### 2. Libraries used (and why)
- Qt 6 (for UI) - by far the easiest to get started with and a good set of features
- Vulkan - very verbose and leaves a lot of room for optimization, especially as OpenGL is becoming more and more outdated. The reason we didn't go for something like DirectX is because the goal is to make it compatible with. every platform (Linux, MacOS and Windows) *Note: this doesn't mean we won't add more in the future*.
### 3. Current scope 
The current goal is to at least get a "minimum viable product", which for us means being able to create a simple game without getting into the grittier details of how graphics API's work and using purely our engine's interface without it feeling under-cooked. Of course there is always more work to do but for now it's just a student project. 


## II. Parallelization
The engine uses a task based system to handle all work. We try to make the engine as non-blocking as possible with real pauses only being between hard dependencies in between tasks. 
***For now*** these dependencies are really simple consisting only of a couple of stages (AKA as many as needed). 

*The `Stage` struct as of now, representing all of the stages and what their **intended** purpose is * 
```cpp
enum class Stage {
PRE_LOAD,
LOAD, // loading resources from disk
AFTER_LOAD,
PRE_INIT,
INIT, // init all the resources
AFTER_INIT,
PRE_UPLOAD,
UPLOAD, // copying them onto API specific buffers and such
AFTER_UPLOAD,
HOT_LOAD, // do something right fucking now
Stage_MAX
};
```

As of now it's only used by the renderer, but as we expand it the need may arise for a more complex system (which of course allow greater parallelization), this, thinking off the top of my head could be done for example, by introducing task-task dependencies.

## III. Resource management
None of this has been fully realized yet, so I am simply going to do broad strokes here.
We will implement and assume a *strictly scene based system*. While this seems like a nothing burger statement, what I mean by that is I don't expect to do much on the fly loading. 
I.%20Components
A user will for example, make something in the engine for a specific scene so we know the set of resources we are going to use in advance. This allows us to preload all of them at once without having to worry about performance (if we loaded them in dynamically resource management becomes a lot more complex as you have to decide what gets loaded in and what doesn't at a moment's notice). 

Any dynamically loaded resources AKA resources that we can't load in advance for one reason or another (e.g. web content), will just be loaded next frame as their performance impact should be minimal.

While this hasn't been implemented yet and will be explained in the next section, it is of note that **all** the resource loading will be delegated to a special `ResourceManager` segment (again a separate shared object file), which will be a part of the main engine and will help with resource sharing between the renderer and master.


## IV. Data oriented design
#### Why?
In our earlier version we had an ECS oriented design that very closely resembled Unity's *(we stole it)*. It seemed very cool in theory and had a lot of neat template hacks to make it work. It was designed well enough and did it's job but it was so **damn annoying** to use, no seriously, fetching components, using them and managing them properly was just cumbersome even if it was good in theory and done by the book. And while this might be an issue from my end there were also concerns about scalability when you had many different objects with many different components (memory fragmentation, mainly).
#### How?
For the uninitiated, data oriented design means there is a fixed size allocated for each and every instance in a "stack layout". Put in Laymen's terms every instance is like a floor in a tower, where each room is an individual component that will exist regardless of whether it's used or not. Just having a known object size comes in very handy when it comes to memory management.

We access the components by instances essentially being just indexes in the stack of components and we can simply use the index and a known offset for any component to access them in O(1)  time.

#### What it allows us to do
So now that we know all the sizes, we can treat all object as equals, which means the entire instance can be stored in contiguous memory. So for instance management we use a Hive/Colony structure (bucketed list), which provides a pretty good middle ground between a list and a vector. *A hive is like a linked list but instead of each node only holding one instance, it holds multiple.* 

## V. Renderer-Master differentiation

As previously mentioned, splitting the engine into multiple chunks was a  big decision, that will hopefully allow for easier swapping of graphics API's and some *very neat* tricks, thanks to them all of them working autonomously.

#### I. Master
The master as mentioned in the [Terms](#Terms), will be the main process. I should clarify this refers to the main non-swappable main shared object file depending on the context *but* it all boils down to the same thing so I won't split hairs here.

The master thread itself won't do much and it's meant more of as an orchestrator for all the tasks, as said previously, we will try our absolute hardest to make it non-blocking unless absolutely necessary.

#### II. Renderer
I see this part more of a second hand to the master. It still handles pretty much the most important task - rendering, and is still working mostly independently. It will have an exposed API the master can interact with and send draw requests to.

#### III. Resource sharing
The resource synchronization (and the assurance of thread safe access) will be managed by the resource manager. It essentially acts as a middle man between the two. I will reiterate the resource manager owns **all** of the "loadable" resources (game assets). It is also of note it owns all of the GPU resources too (things on VRAM like images and models), and simply lends them to the render when deemed safe to do so.

#### IV. Sacrificing memory for performance 
<sub><em>It's not just wholly unoptimized I <b>swear</b></em></sub>

Okay first of all let's just do a bit of simple math on how much memory we need to draw multiple instances of an object so I'm not burned at a stake.

Let's assume we have 10.000 (ten thousand) objects, all the per instance info we need for rendering consists of
```cpp
struct RenderInfo { // this is pseudocode
	vec3 pos;
	vec3 scale;
	vec3_f16 rotation; // a half precision float suffices for rotation
	bool isHidden; // determines whether it should be drawn or not
}
```
all of this takes up a total of 31 bytes. `10.000 instances * 31 bytes = 310.000 bytes`.

Now that we have established the extreme of 10.000 instances only takes up 310 Kb of memory let me get onto my main point.
The resource manager will store n-copies of all the instance data, where n is the number of frame buffers ( e.g. n = 3 for triple buffering). So worst worst case all of the instance takes up ~1 Mb, not a whole lot.

Why? This is the easiest way to allow concurrency while preventing data races and memory corruption. 
While we could simply make the renderer wait for the master and likewise make the master wait, this would destroy all the potential gains we could get from running them simultaneously, instead we simply rotate the data blocks that hold the instance data and allow them both to run concurrently.

#### V.  Data sharing format
Remember at the start where I mention the neat stuff this design allows you to do? This is where it comes into play.

The manager will pass all the necessary info to the renderer in a little packet called `RenderState`.  This allows us to encapsulate *all* of the data needed to render a given frame   into a serializable object that can be saved (apart from all the game assets, which would need to be loaded separately).

Building on top of that we would also have a `GameState` packet which contain *all* the engine info. This again, allows us to create perfect snapshots of a given frame and export/share them.

## VI. Components
As stated before, each and every component is simply a dynamically linked object with an unified interface (they all share common functions for initialization, updating and cleanup) so the engine can use them no matter what. *Right now this is not supported on windows*

If anyone wants to write custom components they can, without recompiling the entire engine while still freely using them in their game, although I must admit the macro fuckery used for this is quite funky and crude.

With each component being separate, we also only export what is necessary so finished products will only have the components they use and nothing more minimizing game size.