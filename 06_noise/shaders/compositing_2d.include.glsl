
float compositing(vec2 coords, int levels) {
	vec2 offset = vec2(1.7, 2.89);
	float factorSum = 0.0;

	float sum = 0.0;
	float octave = 1.0;
	float factor = 1.0;
	for (int i = 1; i <= levels; ++i) {
		sum += factor * noise_2d(octave * coords - i * offset);
		factorSum += factor;
		octave *= 2.0;
		factor *= 0.5;
	}
	return 1.0/factorSum * sum;
}

