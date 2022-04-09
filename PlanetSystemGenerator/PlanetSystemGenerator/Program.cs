                                                                                                                                                                                                                                                                                                                                                              using System;
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

    /// <summary>Body whose position is static and unchanging.</summary>
    class AnchorBody
    {
        /// <summary>Explicitly generates the body.</summary>
        /// <param name="radius">The size of the body when rendered.</param>
        /// <param name="color">The <see cref="Raylib_cs.Color"/> of the body.</param>
        /// <param name="rings">The (optional) system of <see cref="Ring"/>s around the body.</param>
        public AnchorBody(float radius, Color color, Ring[] rings = null)
        {
            Radius = radius;
            Color = color;
            if (rings == null)
                Rings = Array.Empty<Ring>();
            else
                Rings = rings;

            if (Rings.Length > 0) // Kept separate in case an empty array is passed in instead of defaulted.
            {
                RingRenderTex = LoadRenderTexture(256, 256);
                BeginTextureMode(RingRenderTex);
                {
                    Vector2 center = new Vector2(RingTex.width, RingTex.height) / 2;
                    foreach (Ring ring in Rings)
                    {
                        DrawRing(
                            center: center,
                            innerRadius: ring.iRad,
                            outerRadius: ring.oRad,
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
        ~AnchorBody()
        {
            if (Rings.Length > 0)
                UnloadRenderTexture(RingRenderTex);
        }

        /// <summary>Size of body when rendering.</summary>
        public float Radius { get; init; }

        /// <summary>Color of body when rendering.</summary>
        public Color Color { get; init; }

        /// <summary>Data for optional ring system.</summary>
        public Ring[] Rings { get; init; }

        /// <summary>The drawable ring texture.</summary>
        protected RenderTexture2D RingRenderTex { get; init; }
        /// <summary>
        /// The stored 256x256 texture of the <see cref="Ring"/> system, <see cref="Rings">Rings</see>.<br/>
        /// Readonly.
        /// </summary>
        public Texture2D RingTex { get { return RingRenderTex.texture; } }
    }

    /// <summary>A body whose position is calculated relative to the anchor.<br/>Inherits from <see cref="AnchorBody"/>.</summary>
    class MajorBody : AnchorBody
    {
        /// <inheritdoc cref="AnchorBody(float, Color, Ring[])"/>
        /// <param name="orbit">The radius of the orbit.<br/>or<br/>The distance from <paramref name="parent"/> this body orbits at.<br/>(See <see cref="Orbit">Orbit</see>)</param>
        /// <param name="period">Length of a full orbit.<br/>(See <see cref="Period">Period</see>)</param>
        /// <param name="startingT">The initial value of <see cref="T">T</see>.</param>
        public MajorBody(float radius, Color color, Ring[] rings, float orbit, float period, float startingT)
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
        public Vector3 Position { get { return position; } } 
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
    }

    /// <summary>A body whose position is dependent on that of a parent.<br/>Inherits from <see cref="MajorBody"/>.</summary>
    class MinorBody : MajorBody
    {
        /// <inheritdoc cref="MajorBody(float, Color, Ring[], float, float, float)"/>
        /// <param name="parent">The <typeparamref name="MajorBody"/> parent of this body.<br/>(See <see cref="Parent">Parent</see>)</param>
        public MinorBody(float radius, Color color, Ring[] rings, float orbit, float period, float startingT, MajorBody parent)
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
        
        /// <summary>
        /// Generates a random point in a 3D ellipsoid (with X, Y, and Z widths defined by <paramref name="radii"/>) around the <paramref name="center"/>.
        /// </summary>
        /// <param name="center">The point around which the return will be randomly generated (within specified <paramref name="radii"/>).</param>
        /// <param name="radii">The 3D radii around <paramref name="center"/> inside of which the point will be generated.</param>
        /// <returns>A Numerics.Vector3 in a 3D ellipsoid around a specified point.</returns>
        public static Vector3 RandPointInSphere(Vector3 center, Vector3 radii)
        {
            Vector3 offset = new Vector3(
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
        public static Color RandColor_Transparent()
        {
            return new(rnd.Next(256), rnd.Next(256), rnd.Next(256), rnd.Next(256));
        }

        public static void SetTexture(ref Model model, Texture2D texture)
        {
            Material mat = LoadMaterialDefault();
            mat.maps[0] = new MaterialMap { color = Color.WHITE, texture = texture, value = 1.0f };
            model.materials = new Material[] { LoadMaterialDefault() };
            model.materials[0] = LoadMaterialDefault();
        }

        public static void Main()
        {
            InitWindow(1280, 720, "Planet System Generator");
            SetTargetFPS(60);

            float anchorRadius = RandFlt(35, 100);
            int anchorRingCount = rnd.Next(3);
            Ring[] anchorRingSystem = new Ring[anchorRingCount];
            for (int i = 0; i < anchorRingCount; ++i)
            {
                anchorRingSystem[i].iRad = anchorRadius + RandFlt(50, 400);
                anchorRingSystem[i].oRad = anchorRingSystem[i].iRad + RandFlt(10, 100);
                anchorRingSystem[i].color = RandColor_Transparent();
            }
            float anchorAvgTemperature = (anchorRadius - 35) / 100;

            // Star
            AnchorBody anchor = new(
                radius: anchorRadius,
                color: RandColor_Blackbody(Lerp(500, 10000, anchorAvgTemperature), Lerp(700, 12000, anchorAvgTemperature)),
                rings: anchorRingSystem);
            MajorBody[] major; // Planets
            MinorBody[] minor; // Moons

            {
                major = new MajorBody[rnd.Next(3, 25)];
                float lastOrbit = anchor.Radius + RandFlt(50, 200);
                for (int i = 0; i < major.Length; ++i)
                {
                    float bodyRadius = RandFlt(10, 20);
                    float orbitRadius = lastOrbit + RandFlt(10, 80) + bodyRadius;
                    lastOrbit = orbitRadius + bodyRadius;
                    major[i] = new(
                        radius: bodyRadius,
                        color: RandColor_Solid(),
                        rings: new Ring[rnd.Next(7)],
                        orbit: orbitRadius,
                        period: MathF.Pow(orbitRadius, 2) / 5000,
                        startingT: RandFlt(0.0f, 1.0f));
                }
            }

            {
                minor = new MinorBody[rnd.Next(3, 25)];
                for (int i = 0; i < minor.Length; ++i)
                {
                    MajorBody parentBody = major[rnd.Next(major.Length)];
                    float bodyRadius = RandFlt(2, 6);
                    float orbitRadius = parentBody.Radius + bodyRadius + RandFlt(0.1f, 3) * parentBody.Radius;
                    minor[i] = new(
                        radius: bodyRadius,
                        color: RandColor_Solid(),
                        rings: new Ring[rnd.Next(7)],
                        orbit: orbitRadius,
                        period: MathF.Pow(orbitRadius, 2) / 5000,
                        startingT: RandFlt(0.0f, 1.0f),
                        parent: parentBody);
                }
            }

            Mesh mesh = GenMeshPlane(1, 1, 1, 1);
            /// <summary>A generic plane model for drawing ring textures in 3D space.</summary>
            Model ringPlane_generic = LoadModelFromMesh(mesh);
            ringPlane_generic.materials;

            // !! We are using the Z axis for UP !! 

            Camera3D camera;
            camera.position = Vector3.UnitZ * 100;
            camera.target = Vector3.Zero;
            camera.up = Vector3.UnitY;
            camera.fovy = 70;
            camera.projection = CameraProjection.CAMERA_PERSPECTIVE;

            SetCameraMode(camera, CameraMode.CAMERA_FIRST_PERSON);

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
                        DrawSphere(
                            centerPos: Vector3.Zero,
                            radius: anchor.Radius,
                            color: anchor.Color);

                        foreach (MajorBody body in major)
                        {
                            // Orbit
                            DrawCircle3D(
                                center: Vector3.Zero,
                                radius: body.Orbit,
                                rotationAxis: Vector3.UnitZ,
                                rotationAngle: 0.0f,
                                color: Color.BROWN);

                            // Body
                            DrawSphere(
                                centerPos: body.Position,
                                radius: body.Radius,
                                color: body.Color);

                            DrawModel();
                        }
                        foreach (MinorBody body in minor)
                        {
                            // Orbit
                            DrawCircle3D(
                                center: body.Parent.Position,
                                radius: body.Orbit,
                                rotationAxis: Vector3.UnitZ,
                                rotationAngle: 0.0f,
                                color: Color.DARKGRAY);

                            // Body
                            DrawSphere(
                                centerPos: body.Position,
                                radius: body.Radius,
                                color: body.Color);
                        }
                    }
                    EndMode3D();
                }
                EndDrawing();
            }

            CloseWindow();
        }
    }
}
