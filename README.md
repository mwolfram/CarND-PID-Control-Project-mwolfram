# CarND-PID-Control-Project-mwolfram

## Effects of PID parameters

* When setting the **P parameter** alone, I would either end up with too little steering effect or with the car oscillating around the center of the track.
* Setting the **D parameter** additionally reduces oscillation. You can actually see how the steering angle reduces when the car gets closer to the center of the track.
* I "accidentally" tested the **I parameter** (I had D and I mixed up), and saw very clear effects. The car would go completely crazy and leave the track. The I parameter has to be tuned with extreme care. High values let the I term add up to completely mute the other control terms.

## Choice of parameters

* I ended up choosing the parameters manually. I started with tuning P, then added D to reduce the oscillation and did some fine tuning. Apart from the accidental I parameter usage, I mostly ignored the integral term, as the other two already did quite a good job.
* I implemented twiddle as well (can still be found in the code but it's deactivated), but that didn't improve the result much. As the goodness function, I chose the sum of squared CTEs, and if the car left the track, the time until that point plus a bias (to penalize leaving the track).
