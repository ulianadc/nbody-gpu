//
//  kernel.cl
//  NBodySim
//

// Gravitational constant
#define G ((float4)(6.674e-11))

// Represents a single particle in the system
typedef struct {
    
    // Position
    float posx;
    float posy;
    float posz;
    
    // Velocity
    float velx;
    float vely;
    float velz;
    
    // Mass
    float mass;
} Body;

// Main kernel routine
__kernel void nbody(__global Body *curState,       // current state
                    __global Body *nextState,      // next state
                    const unsigned long numBodies) // system size
{
    // Grab unique global work-item id
    unsigned long i = get_global_id(0);
    
    // Proceed only if id is within the system size
    if (i < numBodies) {
        
        // Current position of body i
        float4 pi = (float4)(curState[i].posx, curState[i].posy, curState[i].posz, 0.0f);
        
        // Current velocity of i
        float4 vi = (float4)(curState[i].velx, curState[i].vely, curState[i].velz, 0.0f);
        
        // Mass of i
        float mi = curState[i].mass;
        
        // ...
        float4 pj, u, fTotal, fj;
        float mj, r;
        
        // Initialize total force on i
        fTotal = (float4) 0.0f;
        
        // Iterate through all other bodies
        for (unsigned long j = 0; j < numBodies; j++) {
            
            // Skip body i
            if (j == i)
                continue;
            
            // Get position and mass of j
            pj = (float4)(curState[j].posx, curState[j].posy, curState[j].posz, 0.0f);
            mj = curState[j].mass;
            
            // Square of distance between i and j
            r = pow((pj.x - pi.x), 2) + pow((pj.y - pi.y), 2) + pow((pj.z - pi.z), 2);
            
            // Ignore if j is close to avoid divide by zero
//            if (fabs(r) < 1000.0f)
//                continue;
            
            // Unit vector to j
            u = (pj - pi) / sqrt(r);
            
            // Force vector due to j
            fj = u * ((G * mi * mj) / r);
            
            // Add force due to j to total force on i
            fTotal += fj;
        }
        
        // Compute acceleration on i
        float4 ai = (float4)(fTotal / mi);
        
        // Compute new velocity of i
        float4 viNext = (float4)(vi + ai);
        
        // Compute new position of i
        float4 piNext = (float4)(pi + vi);
        
        // Put together next state of i
        nextState[i].posx = piNext.x;
        nextState[i].posy = piNext.y;
        nextState[i].posz = piNext.z;
        nextState[i].velx = viNext.x;
        nextState[i].vely = viNext.y;
        nextState[i].velz = viNext.z;
        
    }
}