using System;
using System.Runtime.InteropServices;
using System.Numerics;
using Raylib_cs;
using static Raylib_cs.Raylib;
using static Raylib_cs.Raymath;

namespace PlanetSystemGenerator
{
    /// <summary>Stores the data for a single ring.</summary>
    struct Ring
    {
        /// <summary>Inner radius of the ring.</summary>
        public float iRad;
        /// <summary>Outer radius of the ring.</summary>
        public float oRad;
        /// <summary>Color of ring when rendering.</summary>
        public Color color;
    }

    class RingSystem
    {
        static readonly string RingMaskVS =
      @"#version 330
        // Input vertex attributes
        in vec3 vertexPosition;
        in vec2 vertexTexCoord;
        in vec3 vertexNormal;
        in vec4 vertexColor;

        // Input uniform values
        uniform mat4 mvp;

        // Output vertex attributes (to fragment shader)
        out vec2 fragTexCoord;
        out vec4 fragColor;

        // NOTE: Add here your custom variables

        void main()
        {
            // Send vertex attributes to fragment shader
            fragTexCoord = vertexTexCoord;
            fragColor = vertexColor;

            // Calculate final vertex position
            gl_Position = mvp * vec4(vertexPosition, 1.0);
        }";

        static readonly string RingMaskFS =
      @"#version 330

        // Input vertex attributes (from vertex shader)
        in vec2 fragTexCoord;
        in vec4 fragColor;

        // Input uniform values
        uniform sampler2D texture0;
        uniform sampler2D mask;

        // Output fragment color
        out vec4 finalColor;

        void main()
        {
            vec4 texelColor = texture(texture0, fragTexCoord);
            if ((texelColor.r + texelColor.g + texelColor.b) < 0.75) discard;

            finalColor = texelColor;
        }";

        /// <summary>Initializes static members.</summary>
        static RingSystem()
        {
            g_RingModel = LoadModelFromMesh(GenMeshPlane(2, 2, 1, 1));
            //g_RingModel.transform = MatrixRotateXYZ(Vector3.UnitZ);

            unsafe
            {
                g_RingMaskShader = LoadShaderFromMemory(RingMaskVS, RingMaskFS);
                g_RingMaskShader.locs[(int)ShaderLocationIndex.SHADER_LOC_MAP_EMISSION] = GetShaderLocation(g_RingMaskShader, "mask");
            }
        }

        /// <summary>Call at the end of the program to clean up.</summary>
        public static void OkImDoneNow()
        {
            UnloadModel(g_RingModel);
            UnloadShader(g_RingMaskShader);
        }

        /// <summary>Constructs the ring system.</summary>
        /// <param name="rings">Array of <see cref="Ring"/>s.</param>
        /// <exception cref="Exception">Cannot pass Ring[0].</exception>
        public RingSystem(Ring[] rings)
        {
            {
                if (rings.Length == 0)
                    throw new Exception("Passed 0-length array to RingSystem constructor.");

                Rings = rings;
            }

            {
                float maxRingRad = 0;
                foreach (Ring ring in Rings)
                {
                    if (ring.oRad > maxRingRad)
                        maxRingRad = ring.oRad;
                }
                RingRadiusMax = maxRingRad;
            }

            {
                const int textureWidth = 1024;
                RingRenderTex = LoadRenderTexture(textureWidth, textureWidth);

                const int textureHalfWidth = textureWidth / 2;
                Vector2 center = new(textureHalfWidth);

                BeginTextureMode(RingRenderTex);
                {
                    ClearBackground(new(0, 0, 0, 0));

                    foreach (Ring ring in Rings)
                    {
                        DrawRing(
                            center: center,
                            innerRadius: ring.iRad / RingRadiusMax * textureHalfWidth,
                            outerRadius: ring.oRad / RingRadiusMax * textureHalfWidth,
                            startAngle: 0,
                            endAngle: 360,
                            segments: 32,
                            color: ring.color);
                    }
                }
                EndTextureMode();
            }
        }

        /// <summary>Unloads the render texture used for drawing rings.</summary>
        ~RingSystem()
        {
            UnloadRenderTexture(RingRenderTex);
        }

        /// <summary>The plane used for drawing the ring texture.</summary>
        public static Model g_RingModel;
        public static Shader g_RingMaskShader;

        /// <summary>Data for optional ring system.</summary>
        public Ring[] Rings { get; init; }

        public float RingRadiusMax { get; init; }

        /// <summary>The drawable ring texture.</summary>
        private RenderTexture2D RingRenderTex { get; init; }

        /// <summary>
        /// The stored 256x256 texture of the <see cref="Ring"/> system, <see cref="Rings">Rings</see>.<br/>
        /// Readonly.
        /// </summary>
        public Texture2D RingTex { get { return RingRenderTex.texture; } }

        public void Draw(Vector3 position)
        {
            unsafe
            {
                const int materialIndex = 0;

                SetMaterialTexture(
                    material: ref g_RingModel.materials[materialIndex],
                    mapType: MaterialMapIndex.MATERIAL_MAP_DIFFUSE,
                    texture: RingTex);

                SetMaterialShader(
                    model: ref g_RingModel,
                    materialIndex: materialIndex,
                    shader: ref g_RingMaskShader);
            }

            {
                for (int angle = -90; angle <= 90; angle += 180) // Draw 2-sided
                {
                    DrawModelEx(
                        model: g_RingModel,
                        position: position,
                        rotationAxis: Vector3.UnitX,
                        rotationAngle: angle,
                        scale: Vector3.One * RingRadiusMax,
                        tint: Color.WHITE); // No color effects
                }
            }
        }
    }

    /// <summary>Body whose position is static and unchanging.</summary>
    class AnchorBody
    {
        /// <summary>Explicitly generates the body.</summary>
        /// <param name="radius">The size of the body when rendered.</param>
        /// <param name="color">The <see cref="Raylib_cs.Color"/> of the body.</param>
        /// <param name="rings">The (optional) system of <see cref="Ring"/>s around the body.</param>
        public AnchorBody(float radius, Color color, RingSystem rings = null)
        {
            Radius = radius;
            Color = color;
            Rings = rings;
        }

        public virtual Vector3 Position { get { return Vector3.Zero; } }

        /// <summary>Size of body when rendering.</summary>
        public float Radius { get; init; }

        /// <summary>Color of body when rendering.</summary>
        public Color Color { get; init; }

        public RingSystem Rings { get; init; }

        public void DrawRings()
        {
            if (Rings != null)
                Rings.Draw(Position);
        }

        /// <summary>Draws the body</summary>
        public virtual void Draw()
        {
            // Body
            DrawSphere(
                centerPos: Position,
                radius: Radius,
                color: Color);

            DrawRings();
        }
    }

    /// <summary>A body whose position is calculated relative to the anchor.<br/>Inherits from <see cref="AnchorBody"/>.</summary>
    class MajorBody : AnchorBody
    {
        /// <inheritdoc cref="AnchorBody(float, Color, Ring[])"/>
        /// <param name="orbit">The radius of the orbit.<br/>or<br/>The distance from <paramref name="parent"/> this body orbits at.<br/>(See <see cref="Orbit">Orbit</see>)</param>
        /// <param name="period">Length of a full orbit.<br/>(See <see cref="Period">Period</see>)</param>
        /// <param name="startingT">The initial value of <see cref="T">T</see>.</param>
        public MajorBody(float radius, Color color, RingSystem rings, float orbit, float period, float startingT)
            : base(radius, color, rings)
        {
            Orbit = orbit;
            Period = period;
            t = startingT;
            position = Vector3.Zero;
        }

        /// <summary>Size of orbit.</summary>
        /// <value>The distance between the body and its parent.</value>
        public float Orbit { get; init; }

        /// <summary>Length of a full orbit.</summary>
        /// <value>A scalar value (which must be greater than 0) used in the calculation of the body's <see cref="Position">Position</see>.</value>
        public float Period { get; init; }

        /// <summary>How far along its orbit this body is on a percent scale.</summary>
        /// <remarks>Intended for use in a position calculated with <see cref="MathF.Sin(float)"/> and <see cref="MathF.Cos(float)"/>.</remarks>
        /// <value>The Time along the path of the body's orbit, between 0 and 1 inclusively.</value>
        public float T { get { return t; } }
        protected float t;

        /// <summary>Storage of this tick's position to minimize redundant calculation.</summary>
        /// <value>The final <typeparamref name="Vector3"/> position of the body this tick.</value>
        public override Vector3 Position { get { return position; } } 
        protected Vector3 position;

        /// <summary>
        /// Updates the <see cref="Position">position</see> and <see cref="T">time along path</see> of the body for this frame.
        /// </summary>
        /// <remarks>Call this only once per tick.</remarks>
        /// <param name="dt">The delta time since the last tick (time between <see cref="Step"/> calls).</param>
        public virtual void Step(float dt)
        {
            t += dt / Period;
            const float pi2 = MathF.PI * 2;
            float radianT = pi2 * t;
            position.X = MathF.Sin(radianT) * Orbit;
            position.Y = MathF.Cos(radianT) * Orbit;
            position.Z = 0.0f;
        }

        /// <summary>Draws the path of the body's orbit.</summary>
        public virtual void DrawOrbit()
        {
            DrawCircle3D(
                center: Vector3.Zero,
                radius: Orbit,
                rotationAxis: Vector3.UnitZ,
                rotationAngle: 0.0f,
                color: ColorAlpha(Color.GRAY, 0.5f));
        }
    }

    /// <summary>A body whose position is dependent on that of a parent.<br/>Inherits from <see cref="MajorBody"/>.</summary>
    class MinorBody : MajorBody
    {
        /// <inheritdoc cref="MajorBody(float, Color, Ring[], float, float, float)"/>
        /// <param name="parent">The <typeparamref name="MajorBody"/> parent of this body.<br/>(See <see cref="Parent">Parent</see>)</param>
        public MinorBody(float radius, Color color, RingSystem rings, float orbit, float period, float startingT, MajorBody parent)
            : base(radius, color, rings, orbit, period, startingT)
        {
            Parent = parent;
        }

        /// <summary>To whome this body is a satellite of.</summary>
        /// <value>Reference to the <typeparamref name="MajorBody"/> parent whose position this body orbits at a distance of this body's <see cref="MajorBody.Orbit">Orbit</see>.</value>
        public MajorBody Parent { get; init; }

        /// <inheritdoc cref="MajorBody.Step(float)"/>
        public override void Step(float dt)
        {
            base.Step(dt / (MathF.Sqrt(Parent.Radius) * 2));
            position += Parent.Position;
        }

        /// <inheritdoc cref="MajorBody.DrawOrbit()"/>
        public override void DrawOrbit()
        {
            DrawCircle3D(
                center: Parent.Position,
                radius: Orbit,
                rotationAxis: Vector3.UnitZ,
                rotationAngle: 0.0f,
                color: ColorAlpha(Color.DARKGRAY, 0.5f));
        }
    }

    public class Program
    {
        /// <summary>
        /// Turns a <paramref name="fraction"/> into a value between <paramref name="min"/> and <paramref name="max"/> inclusively.<br/>
        /// Equivalent to "<c>Raylib_cs.Lerp(<paramref name="min"/>, <paramref name="max"/>, <paramref name="fraction"/>)</c>".
        /// </summary>
        /// <param name="fraction">A value between 0 and 1 inclusively. Exceeding these bounds is entirely permitted.</param>
        /// <param name="min">The value returned when <paramref name="fraction"/> is 0.</param>
        /// <param name="max">The value returned when <paramref name="fraction"/> is 1.</param>
        /// <returns>A <typeparamref name="float"/> greater than or equal to <paramref name="min"/> and less than or equal to <paramref name="max"/>, proportional to <paramref name="fraction"/>.</returns>
        public static float DecimalToRange(float fraction, float min, float max)
        {
            return fraction * (max - min) + min;
        }

        /// <summary>
        /// Scales a <paramref name="value"/> into a value between 0.0f and 1.0f in proportion to <paramref name="min"/> and <paramref name="max"/>.<br/>
        /// The reverse of "<c>DecimalToRange(<paramref name="value"/>, <paramref name="min"/>, <paramref name="max"/>)</c>".
        /// </summary>
        /// <param name="value">A value between 0 and 1 inclusively. Exceeding these bounds is entirely permitted.</param>
        /// <param name="min">The value returned when <paramref name="value"/> is 0.</param>
        /// <param name="max">The value returned when <paramref name="value"/> is 1.</param>
        /// <returns>A <typeparamref name="float"/> greater than or equal to <paramref name="min"/> and less than or equal to <paramref name="max"/>, proportional to <paramref name="value"/>.</returns>
        public static float RangeToDecimal(float value, float min, float max)
        {
            return (value - min) / (max - min);
        }

        readonly static Random rnd = new();

        /// <summary>Returns a random floating-point precision number that is within the specified range.</summary>
        public static float RandFlt(float min = 0.0f, float max = 1.0f)
        {
            return (float)rnd.NextDouble() * (max - min) + min;
        }

        /// <summary>Returns a random int that is within the specified range. Inclusive for both min and max.</summary>
        public static int RandInt()
        {
            return rnd.Next();
        }
        /// <summary>Returns a random int that is within the specified range. Inclusive for both min and max.</summary>
        public static int RandInt(int max)
        {
            return rnd.Next(max + 1);
        }
        /// <summary>Returns a random int that is within the specified range. Inclusive for both min and max.</summary>
        public static int RandInt(int min, int max)
        {
            return rnd.Next(min, max + 1);
        }
        
        /// <summary>
        /// Generates a random point in a 3D ellipsoid (with X, Y, and Z widths defined by <paramref name="radii"/>) around the <paramref name="center"/>.
        /// </summary>
        /// <param name="center">The point around which the return will be randomly generated (within specified <paramref name="radii"/>).</param>
        /// <param name="radii">The 3D radii around <paramref name="center"/> inside of which the point will be generated.</param>
        /// <returns>A Numerics.Vector3 in a 3D ellipsoid around a specified point.</returns>
        public static Vector3 RandPointInSphere(Vector3 center, Vector3 radii)
        {
            Vector3 offset = new(
                RandFlt(-1.0f, 1.0f),
                RandFlt(-1.0f, 1.0f),
                RandFlt(-1.0f, 1.0f));
            return center + Vector3.Normalize(offset) * radii;
        }


        /// <summary>
        /// Returns a color based on blackbody radiation with a temperature (inclusively) between <paramref name="minTemp"/> and <paramref name="maxTemp"/>.
        /// </summary>
        /// <param name="minTemp">The (inclusive) minimum temperature that can be generated randomly, measured in Kelvin.</param>
        /// <param name="maxTemp">The (inclusive) maximum temperature that can be generated randomly, measured in Kelvin.</param>
        /// <returns>An instance of Raylib_cs.Color, on the blackbody scale.</returns>
        public static Color RandColor_Blackbody(float minTemp = 500, float maxTemp = 12000)
        {
            float temperature = RandFlt(minTemp, maxTemp);
            float amount = (temperature - 500) / (12000 - 500);
            Color color;
            // Uses cubic bezier curves to calculate blackbody color.
            // I completely made up these numbers btw; they are not scientifically accurate.
            color.r = (byte)Lerp(Lerp(255, 255, amount), Lerp(255, 175, amount), amount);
            color.g = (byte)Lerp(Lerp(55, 250, amount), Lerp(250, 200, amount), amount);
            color.b = (byte)Lerp(Lerp(0, 200, amount), Lerp(200, 255, amount), amount);
            color.a = 255;
            return color;
        }
        /// <summary>
        /// Generates a completely random color with full opacity.
        /// </summary>
        /// <returns>An instance of Raylib_cs.Color with an alpha of 255.</returns>
        public static Color RandColor_Solid()
        {
            return new(rnd.Next(256), rnd.Next(256), rnd.Next(256), a: 255);
        }
        /// <summary>
        /// Generates a completely random color with random opacity.
        /// </summary>
        /// <returns>An instance of Raylib_cs.Color.</returns>
        public static Color RandColor_Transparent(int minAlpha = 0, int maxAlpha = 256)
        {
            return new(rnd.Next(256), rnd.Next(256), rnd.Next(256), rnd.Next(minAlpha, maxAlpha));
        }

        static readonly float AnchorRadiusMin = 35, AnchorRadiusMax = 100;
        static readonly float SmallStarTemperatureMin = 500, SmallStarTemperatureMax = 700;
        static readonly float LargeStarTemperatureMin = 10000, LargeStarTemperatureMax = 12000;

            static readonly int AsteroidBeltCountMax = 3;
            static readonly float AsteroidBeltDistanceMin = 50, AsteroidBeltDistanceMax = 400;
            static readonly float AsteroidBeltWidthMin = 10, AsteroidBeltWidthMax = 100;

        static readonly int MajorBodyCountMin = 3, MajorBodyCountMax = 25;
        static readonly float MajorBodyRadiusMin = 10, MajorBodyRadiusMax = 20;
        static readonly float ClosestMajorBodyOrbitRadiusMin = 50, ClosestMajorBodyOrbitRadiusMax = 200;
        static readonly float MajorBodyOrbitalSeparationMin = 10, MajorBodyOrbitalSeparationMax = 80;

            static readonly int MajorBodyRingCountMax = 5;
            static readonly float MajorBodyRingDistanceMin = 1, MajorBodyRingDistanceMax = 20;
            static readonly float MajorBodyRingWidthMin = 1, MajorBodyRingWidthMax = 10;

        static readonly int MinorBodyCountMin = 6, MinorBodyCountMax = 64;
        static readonly float MinorBodyRadiusMin = 2, MinorBodyRadiusMax = 6;
        static readonly float MinorBodyOrbitRadiusMin = 0.1f, MinorBodyOrbitRadiusMax = 3;

            static readonly int MinorBodyRingCountMax = 3;
            static readonly float MinorBodyRingDistanceMin = 0.3f, MinorBodyRingDistanceMax = 5;
            static readonly float MinorBodyRingWidthMin = 0.5f, MinorBodyRingWidthMax = 7;

        public static void Main()
        {
            InitWindow(1280, 720, "Planet System Generator");
            SetTargetFPS(60);

            // Star
            AnchorBody anchor;
            MajorBody[] major; // Planets
            MinorBody[] minor; // Moons

            // Anchor
            {
                float bodyRadius = RandFlt(AnchorRadiusMin, AnchorRadiusMax);

                // Ring gen
                int ringCount = RandInt(AsteroidBeltCountMax);
                Ring[] ringSystem = new Ring[ringCount];
                for (int i = 0; i < ringCount; ++i)
                {
                    ringSystem[i].iRad = bodyRadius + RandFlt(AsteroidBeltDistanceMin, AsteroidBeltDistanceMax);
                    ringSystem[i].oRad = ringSystem[i].iRad + RandFlt(AsteroidBeltWidthMin, AsteroidBeltWidthMax);
                    ringSystem[i].color = RandColor_Transparent(minAlpha: 128);
                }

                float anchorDecimalRadius = RangeToDecimal(bodyRadius, AnchorRadiusMin, AnchorRadiusMax);
                anchor = new(
                    radius: bodyRadius,
                    rings: (ringCount > 0) ? new RingSystem(ringSystem) : null,
                    color: RandColor_Blackbody(
                        minTemp: DecimalToRange(anchorDecimalRadius, SmallStarTemperatureMin, SmallStarTemperatureMax),
                        maxTemp: DecimalToRange(anchorDecimalRadius, LargeStarTemperatureMin, LargeStarTemperatureMax)));
            }

            // Major bodies
            {
                major = new MajorBody[RandInt(MajorBodyCountMin, MajorBodyCountMax)];
                float lastOrbit = anchor.Radius + RandFlt(ClosestMajorBodyOrbitRadiusMin, ClosestMajorBodyOrbitRadiusMax);
                for (int i = 0; i < major.Length; ++i)
                {
                    float bodyRadius = RandFlt(MajorBodyRadiusMin, MajorBodyRadiusMax);
                    float orbitRadius = lastOrbit + RandFlt(MajorBodyOrbitalSeparationMin, MajorBodyOrbitalSeparationMax) + bodyRadius;
                    lastOrbit = orbitRadius + bodyRadius;

                    // Ring gen
                    int ringCount = RandInt(MajorBodyRingCountMax);
                    Ring[] ringSystem = new Ring[ringCount];
                    for (int j = 0; j < ringCount; ++j)
                    {
                        ringSystem[j].iRad = bodyRadius + RandFlt(MajorBodyRingDistanceMin, MajorBodyRingDistanceMax);
                        ringSystem[j].oRad = ringSystem[j].iRad + RandFlt(MajorBodyRingWidthMin, MajorBodyRingWidthMax);
                        ringSystem[j].color = RandColor_Transparent(minAlpha: 128);
                    }

                    major[i] = new(
                        radius: bodyRadius,
                        color: RandColor_Solid(),
                        rings: (ringCount > 0) ? new RingSystem(ringSystem) : null,
                        orbit: orbitRadius,
                        period: MathF.Pow(orbitRadius, 2) / 5000,
                        startingT: RandFlt()); // 0..1
                }
            }

            // Minor bodies
            {
                minor = new MinorBody[RandInt(MinorBodyCountMin, MinorBodyCountMax)];
                for (int i = 0; i < minor.Length; ++i)
                {
                    MajorBody parentBody = major[rnd.Next(major.Length)]; // Select a major body randomly
                    float bodyRadius = RandFlt(MinorBodyRadiusMin, MinorBodyRadiusMax);
                    float orbitRadius = parentBody.Radius + bodyRadius + RandFlt(MinorBodyOrbitRadiusMin, MinorBodyOrbitRadiusMax) * parentBody.Radius;

                    // Ring gen
                    int ringCount = rnd.Next(MinorBodyRingCountMax);
                    Ring[] ringSystem = new Ring[ringCount];
                    for (int j = 0; j < ringCount; ++j)
                    {
                        ringSystem[j].iRad = bodyRadius + RandFlt(MinorBodyRingDistanceMin, MinorBodyRingDistanceMax);
                        ringSystem[j].oRad = ringSystem[j].iRad + RandFlt(MinorBodyRingWidthMin, MinorBodyRingWidthMax);
                        ringSystem[j].color = RandColor_Transparent(minAlpha: 128);
                    }

                    minor[i] = new(
                        radius: bodyRadius,
                        color: RandColor_Solid(),
                        rings: (ringCount > 0) ? new RingSystem(ringSystem) : null,
                        orbit: orbitRadius,
                        period: MathF.Pow(orbitRadius, 2) / 5000,
                        startingT: RandFlt(), // 0..1
                        parent: parentBody);
                }
            }

            // !! We are using the Z axis for UP !! 

            Camera3D camera;
            camera.position = Vector3.UnitZ * AnchorRadiusMax;
            camera.target = Vector3.Zero;
            camera.up = Vector3.UnitY;
            camera.fovy = 45;
            camera.projection = CameraProjection.CAMERA_PERSPECTIVE;

            SetCameraMode(camera, CameraMode.CAMERA_ORBITAL);

            while (!WindowShouldClose())
            {
                float dt = GetFrameTime();
                foreach (MajorBody body in major)
                {
                    body.Step(dt);
                }
                foreach (MajorBody body in minor)
                {
                    body.Step(dt);
                }

                UpdateCamera(ref camera);

                BeginDrawing();
                {
                    ClearBackground(Color.BLACK);

                    BeginMode3D(camera);
                    {
                        // Body
                        anchor.Draw();

                        foreach (MajorBody body in major)
                        {
                            // Orbit
                            body.DrawOrbit();

                            // Body
                            body.Draw();
                        }
                        foreach (MinorBody body in minor)
                        {
                            // Orbit
                            body.DrawOrbit();

                            // Body
                            body.Draw();
                        }
                    }
                    EndMode3D();
                }
                EndDrawing();
            }

            RingSystem.OkImDoneNow();

            CloseWindow();
        }
    }
}
