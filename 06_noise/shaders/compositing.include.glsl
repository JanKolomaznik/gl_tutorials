
float compositing(vec3 coords, int levels) {
	vec3 offset = vec3(1.7, 2.89, 9.3);
	float factorSum = 0.0;

	float sum = 0.0;
	float octave = 1.0;
	float factor = 1.0;
	for (int i = 1; i <= levels; ++i) {
		sum += factor * noise_3d(octave * coords - i * offset);
		factorSum += factor;
		octave *= 2.0;
		factor *= 0.5;
	}
	return 1.0/factorSum * sum;
}

