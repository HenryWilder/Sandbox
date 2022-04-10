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
        public RingSystem(Ring[] rings, float angle)
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
                            segments: 128,
                            color: ring.color);
                    }
                }
                EndTextureMode();
            }

            Angle = angle;
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

        public float Angle { get; init; }

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
                        rotationAngle: angle + Angle,
                        scale: Vector3.One * RingRadiusMax,
                        tint: Color.WHITE); // No color effects
                }
            }
        }
    }

    interface IHoverable
    {
        bool Hovered(Ray ray);
        void Highlight(Color highlight, float scale = 1);
    }

    /// <summary>Body whose position is static and unchanging.</summary>
    class AnchorBody : IHoverable
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
        public void Draw()
        {
            // Body
            DrawSphere(Position, Radius, Color);

            DrawRings();
        }

        public bool Hovered(Ray ray)
        {
            CBool result = GetRayCollisionSphere(ray, Position, Radius);
            return result;
        }

        public void Highlight(Color highlight, float Scale = 1)
        {
            DrawSphere(Position, Radius * Scale, highlight);
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
        /// <summary>Draws a line from the body to its parent.</summary>
        public virtual void DrawParentLine()
        {
            DrawLine3D(Position, Vector3.Zero, ColorAlpha(Color.GRAY, 0.5f));
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
        /// <inheritdoc cref="MajorBody.DrawParentLine()"/>
        public override void DrawParentLine()
        {
            DrawLine3D(Position, Parent.Position, ColorAlpha(Color.DARKGRAY, 0.5f));
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

        /// <summary>Returns true with a one out of <paramref name="x"/> chance.</summary>
        public static bool OneIn(int x)
        {
            return rnd.Next(x) == 0;
        }

        #region Universal constants
        const float UnitScale = 0.25f;
        #endregion // Universal constants

        #region Anchor constants
        const float
            AnchorRadiusMin = 3.5f * UnitScale,
            AnchorRadiusMax = 10 * UnitScale;
        const float
            SmallStarTemperatureMin = 500 * UnitScale,
            SmallStarTemperatureMax = 700 * UnitScale;
        const float
            LargeStarTemperatureMin = 10000 * UnitScale,
            LargeStarTemperatureMax = 12000 * UnitScale;

            #region Asteroid belt constants
            const int
                AsteroidBeltCountMax = 3;
            const float
                AsteroidBeltDistanceMin = 5 * UnitScale,
                AsteroidBeltDistanceMax = 40 * UnitScale;
            const float 
                AsteroidBeltWidthMin = 1 * UnitScale,
                AsteroidBeltWidthMax = 10 * UnitScale;
            #endregion // Asteroid belt constants

        #endregion // Anchor constants

        #region Major body constants
        const int
            MajorBodyCountMin = 3,
            MajorBodyCountMax = 25;
        const float
            MajorBodyRadiusMin = 1 * UnitScale,
            MajorBodyRadiusMax = 2 * UnitScale;
        const float
            ClosestMajorBodyOrbitRadiusMin = 5 * UnitScale,
            ClosestMajorBodyOrbitRadiusMax = 20 * UnitScale;
        const float
            MajorBodyOrbitalSeparationMin = 1 * UnitScale,
            MajorBodyOrbitalSeparationMax = 8 * UnitScale;

            #region Major body ring constants
            const int
                MajorBodyRingCountMax = 5;
            const float
                MajorBodyRingDistanceMin = 0.1f * UnitScale,
                MajorBodyRingDistanceMax = 2 * UnitScale;
            const float
                MajorBodyRingWidthMin = 0.1f * UnitScale,
                MajorBodyRingWidthMax = 1 * UnitScale;
            #endregion // Major body ring constants

        #endregion // Major body constants

        #region Minor body constants
        const int
            MinorBodyCountMin = 6,
            MinorBodyCountMax = 64;
        const float
            MinorBodyRadiusMin = 0.2f * UnitScale,
            MinorBodyRadiusMax = 0.6f * UnitScale;
        const float
            MinorBodyOrbitRadiusMin = 0.01f * UnitScale,
            MinorBodyOrbitRadiusMax = 0.3f * UnitScale;

            #region Minor body ring constants
            const int
                MinorBodyRingCountMax = 3;
            const float
                MinorBodyRingDistanceMin = 0.03f * UnitScale,
                MinorBodyRingDistanceMax = 0.5f * UnitScale;
            const float
                MinorBodyRingWidthMin = 0.05f * UnitScale,
                MinorBodyRingWidthMax = 0.7f * UnitScale;
            #endregion // Minor body ring constants

        #endregion // Minor body constants

        enum AnchorType
        {
            DwarfStar,
            NormalStar,
            NeutronStar,
            Pulsar,
            BlackHole,
            ProtoStar,
        }
        enum PlanetType
        {
            GasGiant,
            Rocky,
            Icey, // Far from the star
            Scorched, // Close to the star
        }
        enum MoonType
        {
            Rocky,
            GasTiny, // I made this one up
        }

        public static void Main()
        {
            #region Setup phase

            InitWindow(1280, 720, "Planet System Generator");
            SetTargetFPS(60);

            #region Body Generation

            AnchorBody anchor; // Star
            MajorBody[] major; // Planets
            MinorBody[] minor; // Moons
            #region Anchor
            {
                AnchorType anchorType = AnchorType.NormalStar;
#if false
            {
                double roll = rnd.NextDouble();

                if (roll <= 0.0001)
                    anchorType = AnchorType.ProtoStar;
                else if (roll <= 0.001)
                    anchorType = AnchorType.BlackHole;
                else if (roll <= 0.05)
                    anchorType = AnchorType.Pulsar;
                else if (roll <= 0.1)
                    anchorType = AnchorType.NeutronStar;
                else if (roll <= 0.5)
                    anchorType = AnchorType.DwarfStar;
                else
                    anchorType = AnchorType.NormalStar;
            }
#endif
                switch (anchorType)
                {
                    case AnchorType.ProtoStar:
                        {
                            // Todo
                            anchor = new(
                                radius: AnchorRadiusMin,
                                color: Color.MAGENTA,
                                rings: null);
                        }
                        break;

                    case AnchorType.BlackHole:
                        {
                            float bodyRadius = MajorBodyRadiusMin;

                            // Ring gen
                            int ringCount = 10;
                            Ring[] ringSystem = new Ring[ringCount];
                            float lastEnd = bodyRadius;
                            for (int i = 0; i < ringCount; ++i)
                            {
                                float dist = RandFlt();
                                ringSystem[i].iRad = lastEnd;
                                lastEnd = ringSystem[i].oRad = ringSystem[i].iRad + DecimalToRange(dist, MajorBodyRingWidthMin, MajorBodyRingWidthMax);
                                float amount = i / 10.0f;
                                Vector3 colorVec;
                                if (i == 0)
                                    colorVec = Vector3.One;
                                else
                                    colorVec = Vector3Lerp(Vector3.UnitX + Vector3.UnitY, Vector3.UnitX, amount);
                                ringSystem[i].color = new Color((int)(colorVec.X * 255), (int)(colorVec.Y * 255), (int)(colorVec.Z * 255), (int)DecimalToRange(1 - amount, 190, 255));
                            }

                            anchor = new(
                                radius: bodyRadius,
                                color: Color.BLACK,
                                rings: new RingSystem(ringSystem, 0));
                        }
                        break;

                    case AnchorType.Pulsar:
                        {
                            // Todo
                            anchor = new(
                                radius: AnchorRadiusMin,
                                color: Color.MAGENTA,
                                rings: null);
                        }
                        break;

                    case AnchorType.NeutronStar:
                        {
                            // Todo
                            anchor = new(
                                radius: AnchorRadiusMin,
                                color: Color.MAGENTA,
                                rings: null);
                        }
                        break;

                    case AnchorType.DwarfStar:
                        {
                            // Todo
                            anchor = new(
                                radius: AnchorRadiusMin,
                                color: Color.MAGENTA,
                                rings: null);
                        }
                        break;

                    default:
                    case AnchorType.NormalStar:
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
                                ringSystem[i].color.b = ringSystem[i].color.g = ringSystem[i].color.r; // Yellowish asteroid belts
                            }

                            float anchorDecimalRadius = RangeToDecimal(bodyRadius, AnchorRadiusMin, AnchorRadiusMax);
                            Color color = RandColor_Blackbody(
                                    minTemp: DecimalToRange(anchorDecimalRadius, SmallStarTemperatureMin, SmallStarTemperatureMax),
                                    maxTemp: DecimalToRange(anchorDecimalRadius, LargeStarTemperatureMin, LargeStarTemperatureMax));

                            anchor = new(
                                radius: bodyRadius,
                                color: color,
                                rings: (ringCount > 0) ? new RingSystem(ringSystem, RandFlt(-5,5)) : null);
                        }
                        break;
                }
            }
#endregion // Anchor

#region Major bodies
            {
                major = new MajorBody[RandInt(MajorBodyCountMin, MajorBodyCountMax)];
                float lastOrbit = anchor.Radius + RandFlt(ClosestMajorBodyOrbitRadiusMin, ClosestMajorBodyOrbitRadiusMax);
                for (int i = 0; i < major.Length; ++i)
                {
                    float bodyRadius = RandFlt(MajorBodyRadiusMin, MajorBodyRadiusMax);
                    float orbitRadius = lastOrbit + RandFlt(MajorBodyOrbitalSeparationMin, MajorBodyOrbitalSeparationMax) + bodyRadius;
                    lastOrbit = orbitRadius + bodyRadius;

                    // Ring gen
                    int[] chanceOfRings = { 3, 2, 4, 6, 8 }; // Lower number = higher probability
                    int ringCount = 0;
                    for (int ring = 0; ring < MajorBodyRingCountMax; ++ring)
                    {
                        if (OneIn(chanceOfRings[ring]))
                            ringCount++;
                    }
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
                        rings: (ringCount > 0) ? new RingSystem(ringSystem, MathF.Pow(RandFlt(-1,1),10) * 90) : null,
                        orbit: orbitRadius,
                        period: orbitRadius / UnitScale,
                        startingT: RandFlt()); // 0..1
                }
            }
#endregion // Major bodies

#region Minor bodies
            {
                minor = new MinorBody[RandInt(MinorBodyCountMin, MinorBodyCountMax)];
                for (int i = 0; i < minor.Length; ++i)
                {
                    MajorBody parentBody = major[rnd.Next(major.Length)]; // Select a major body randomly
                    float bodyRadius = RandFlt(MinorBodyRadiusMin, MinorBodyRadiusMax);
                    float orbitRadius = parentBody.Radius + bodyRadius + RandFlt(MinorBodyOrbitRadiusMin, MinorBodyOrbitRadiusMax) + parentBody.Radius;

                    // Ring gen
                    int ringCount = OneIn(50) ? RandInt(MinorBodyRingCountMax) : 0;
                    Ring[] ringSystem = new Ring[ringCount];
                    for (int j = 0; j < ringCount; ++j)
                    {
                        ringSystem[j].iRad = bodyRadius * 2 + RandFlt(MinorBodyRingDistanceMin, MinorBodyRingDistanceMax);
                        ringSystem[j].oRad = ringSystem[j].iRad + RandFlt(MinorBodyRingWidthMin, MinorBodyRingWidthMax);
                        ringSystem[j].color = RandColor_Transparent(minAlpha: 128);
                    }

                    minor[i] = new(
                        radius: bodyRadius,
                        color: RandColor_Solid(),
                        rings: (ringCount > 0) ? new RingSystem(ringSystem, RandFlt(-45,45)) : null,
                        orbit: orbitRadius,
                        period: orbitRadius / UnitScale,
                        startingT: RandFlt(), // 0..1
                        parent: parentBody);
                }
            }
            #endregion // Minor bodies

            #endregion // Body generation

            IHoverable[] hoverable = new IHoverable[1 + major.Length + minor.Length];
            {
                int i = 0;
                hoverable[i++] = anchor;
                foreach (MajorBody body in major)
                {
                    hoverable[i++] = body;
                }
                foreach (MinorBody body in minor)
                {
                    hoverable[i++] = body;
                }
            }
            IHoverable hovered;

            // !! We are using the Z axis for UP !!
            // Note: by "up", I mean the up axis of the system (perpendicular to orbits), not the screen.

            Camera3D camera; {
                camera.position = Vector3.UnitZ * AnchorRadiusMax * 2;
                camera.target = Vector3.Zero;
                camera.up = Vector3.UnitY;
                camera.fovy = 45;
                camera.projection = CameraProjection.CAMERA_PERSPECTIVE;
            }
            SetCameraMode(camera, CameraMode.CAMERA_ORBITAL);

#endregion // Setup phase

            #region Game loop
            while (!WindowShouldClose())
            {
                #region Simulation phase
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

                Ray mouseRay = GetMouseRay(GetMousePosition(), camera);

                hovered = null;
                foreach (IHoverable body in hoverable)
                {
                    if (body.Hovered(mouseRay))
                    {
                        hovered = body;
                        break;
                    }
                }

                #endregion // Simulation phase

                #region Draw phase
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
                            //body.DrawOrbit();
                            body.DrawParentLine();

                            // Body
                            body.Draw();
                        }
                        foreach (MinorBody body in minor)
                        {
                            // Orbit
                            //body.DrawOrbit();
                            body.DrawParentLine();

                            // Body
                            body.Draw();
                        }

                        if (hovered != null)
                            hovered.Highlight(Color.YELLOW, 1.1f);

                        DrawRay(mouseRay, Color.RED);
                    }
                    EndMode3D();
                }
                EndDrawing();
                #endregion // Draw phase
            }
            #endregion // Game loop

            #region Cleanup phase

            hovered = null;
            hoverable = Array.Empty<IHoverable>();

            RingSystem.OkImDoneNow(); // Release static RingSystem members

            CloseWindow();

            #endregion // Cleanup phase
        }
        // Exception happens here
    }
}
